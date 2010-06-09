function init(canvas, nodejs) {
    var MAX_FPS = 60.0;

    var gl = canvas.getContext('experimental-webgl');

    if(nodejs) {
	var webgl = require('../webgl');
	ArrayBuffer = require('buffer').Buffer;
	FloatArray = webgl.FloatArray;
	Image = webgl.Image;
    }

    // Shaders

    var program = gl.createProgram();
    (function() {
	var vsource = [
            "uniform mat4 uProjectionMatrix;",
	    "attribute vec3 aVertexPosition;",
	    "attribute vec2 aTextureCoord;",
	    "",
	    "varying vec2 vTextureCoord;",
	    "",
	    "void main(void) {",
            "    gl_Position = uProjectionMatrix * vec4(aVertexPosition, 1.0);",
	    "",
	    "    vTextureCoord = aTextureCoord;",
	    "}"
	].join("\n");
	
	var fsource = [
	    "varying vec2 vTextureCoord;",
	    "",
	    "uniform sampler2D uSampler;",
	    "",
	    "void main(void) {",
	    "    gl_FragColor = texture2D(uSampler, vTextureCoord);",
	    "}"
	].join("\n");
	
	var vshader = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vshader, vsource);
	gl.compileShader(vshader);

	var fshader = gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fshader, fsource);
	gl.compileShader(fshader);

	gl.attachShader(program, vshader);
	gl.attachShader(program, fshader);
	
	gl.linkProgram(program);
	gl.useProgram(program);
    })();

    // Texture

    var textureCoordBuffer = gl.createBuffer();
    var textureArray;
    var textureFloatArray;
    try {
	textureArray = new ArrayBuffer(2 * 4 * 4);
	textureFloatArray = new FloatArray(textureArray);
    } catch (x) {
    }
    var textureCoordAttribute = gl.getAttribLocation(program, "aTextureCoord");
    gl.enableVertexAttribArray(textureCoordAttribute);
    (function() {
	var textureCoord = [
	    1.0, 1.0,
	    0.0, 1.0,
	    1.0, 0.0,
	    0.0, 0.0
	];

	try {
	    textureFloatArray.set(textureCoord);
	} catch (x) {
	    textureFloatArray = new WebGLFloatArray(textureCoord);
	}
	
	gl.bindBuffer(gl.ARRAY_BUFFER, textureCoordBuffer);
	gl.bufferData(gl.ARRAY_BUFFER, textureFloatArray, gl.STATIC_DRAW);
    })();

    var halfWidth = 0;
    var halfHeight = 0;

    var logo = new Image();
    var texture = gl.createTexture();
    function loadTexture() {
	halfWidth = logo.width / 2;
	halfHeight = logo.height / 2;

        gl.bindTexture(gl.TEXTURE_2D, texture);
	gl.texImage2D(gl.TEXTURE_2D, 0, logo, true);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    }
    logo.addEventListener('load', loadTexture);
    var filename = "node_logo.png";
    if(nodejs) {
	// Might be executing from any path.
	filename = __dirname + "/" + filename;
    }
    logo.src = filename;

    // Vertices

    var vertexBuffer = gl.createBuffer();
    var vertexArray;
    var vertexFloatArray;
    try {
	vertexArray = new ArrayBuffer(4 * 4 * 3);
	vertexFloatArray = new FloatArray(vertexArray);
    } catch (x) {
    }
    var vertexPositionAttribute = gl.getAttribLocation(program, "aVertexPosition");
    gl.enableVertexAttribArray(vertexPositionAttribute);
    function setVerts(x, y) {

	var vertices = [
	    x + halfWidth, y + halfHeight, 0.0,
	    x + -halfWidth, y + halfHeight, 0.0,
	    x + halfWidth, y + -halfHeight, 0.0,
	    x + -halfWidth, y + -halfHeight, 0.0
	];

	try {
	    vertexFloatArray.set(vertices);
	} catch (x) {
	    vertexFloatArray = new WebGLFloatArray(vertices);
	}
	
	gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
	gl.bufferData(gl.ARRAY_BUFFER, vertexFloatArray, gl.STATIC_DRAW);
    }

    // Projection matrix

    var projectionArray;
    var projectionFloatArray;
    try {
	projectionArray = new ArrayBuffer(4 * 16);
	projectionFloatArray = new FloatArray(projectionArray);
    } catch (x) {
    }
    (function() {
        var left = 0;
        var right = canvas.width;
        var bottom = 0;
        var top = canvas.height;
        var near = 1;
        var far = -1;

        var tx = -(right + left) / (right - left);
        var ty = -(top + bottom) / (top - bottom);
        var tz = -(far + near) / (far - near);
        
        var orthoMatrix = [];
        orthoMatrix[0] = 2 / (right - left);
        orthoMatrix[1] = 0;
        orthoMatrix[2] = 0;
        orthoMatrix[3] = 0;
        orthoMatrix[4] = 0;
        orthoMatrix[5] = 2 / (top - bottom);
        orthoMatrix[6] = 0;
        orthoMatrix[7] = 0;
        orthoMatrix[8] = 0;
        orthoMatrix[9] = 0;
        orthoMatrix[10] = -2 / (far - near);
        orthoMatrix[11] = 0;
        orthoMatrix[12] = tx;
        orthoMatrix[13] = ty;
        orthoMatrix[14] = tz;
        orthoMatrix[15] = 1;

	try {
	    projectionFloatArray.set(orthoMatrix);
	} catch (x) {
	    projectionFloatArray = new WebGLFloatArray(orthoMatrix);
	}

        var uProjectionMatrix = gl.getUniformLocation(program, "uProjectionMatrix");
        gl.uniformMatrix4fv(uProjectionMatrix, false, projectionFloatArray);
    })();

    // Events

    var mouseover, mousex, mousey;
    function mousemove(e) {
        mousex = e.offsetX;
	mousey = e.offsetY;
    }

    canvas.addEventListener('quit', function() {
        process.exit(); // node-webgl has a special quit event
    });
    canvas.addEventListener('mousemove', mousemove);

    // Background animation
    var bg = [0, 0, 0];
    function changeBackground() {
	bg[0] += -0.005 + (Math.random() / 100);
	bg[1] += -0.005 + (Math.random() / 100);
	bg[2] += -0.005 + (Math.random() / 100);

	for(var i = 0; i < 3; i++) {
	    if(bg[i] < 0) bg[i] = 0;
	    if(bg[i] > 1) bg[i] = 1;
	}
    }

    // Draw function

    var sampler = gl.getUniformLocation(program, "uSampler"),
        x = canvas.width / 2,
        y = canvas.height / 2,
        xspeed = 50,
        yspeed = 50,
        lastDrawTime = (new Date()).getTime();

    function draw() {
        var currentTime = (new Date()).getTime();
        var dt = (currentTime - lastDrawTime) / 1000;

	if(x - halfWidth < 0) {
	    x = halfWidth;
	    xspeed = -xspeed;
	} else if(x + halfWidth > canvas.width) {
	    x = canvas.width - halfWidth;
	    xspeed = -xspeed;
	}

	if(y - halfHeight < 0) {
	    y = halfHeight;
	    yspeed = -yspeed;
	} else if(y + halfHeight > canvas.height) {
	    y = canvas.height - halfHeight;
	    yspeed = -yspeed;
	}

	x += xspeed * dt;
	y += yspeed * dt;

	setVerts(x, y);

	changeBackground();

	gl.clearColor(bg[0], bg[1], bg[2], 1);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	
	gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
	gl.vertexAttribPointer(vertexPositionAttribute, 3, gl.FLOAT, false, 0, 0);
	
	gl.bindBuffer(gl.ARRAY_BUFFER, textureCoordBuffer);
	gl.vertexAttribPointer(textureCoordAttribute, 2, gl.FLOAT, false, 0, 0);
	
	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D, texture);
	gl.uniform1i(sampler, 0);
	
	gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);

	// At one point of the WebGL spec, swapBuffers was implemented
	// node-webgl also needs it at the moment
	if(gl.swapBuffers) gl.swapBuffers();
	
        lastDrawTime = currentTime;

	setTimeout(draw, 1000 / MAX_FPS);
    };
    draw();
}

if(typeof exports !== 'undefined') {
    exports.init = init;
}
