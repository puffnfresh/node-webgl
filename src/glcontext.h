#include <SDL/SDL_opengl.h>

#include <v8.h>

#include <node.h>
#include <node_events.h>

#include "typedarray.h"
#include "image.h"

using namespace v8;
using namespace node;

class GLContext : public ObjectWrap {
public:

    static void
    Initialize (Handle<Object> target) {
        HandleScope scope;

        Local<FunctionTemplate> t = FunctionTemplate::New(New);
	constructor_template = Persistent<FunctionTemplate>::New(t);

        t->InstanceTemplate()->SetInternalFieldCount(1);

	// Constants
	Handle<ObjectTemplate> proto = t->PrototypeTemplate();

	SetConstant(proto, "RGB", 0x1907);
	SetConstant(proto, "RGBA", 0x1908);
	
	SetConstant(proto, "DEPTH_BUFFER_BIT",   0x00000100);
	SetConstant(proto, "STENCIL_BUFFER_BIT", 0x00000400);
	SetConstant(proto, "COLOR_BUFFER_BIT",   0x00004000);

	SetConstant(proto, "POINTS",         0x0000);
	SetConstant(proto, "LINES",          0x0001);
	SetConstant(proto, "LINE_LOOP",      0x0002);
	SetConstant(proto, "LINE_STRIP",     0x0003);
	SetConstant(proto, "TRIANGLES",      0x0004);
	SetConstant(proto, "TRIANGLE_STRIP", 0x0005);
	SetConstant(proto, "TRIANGLE_FAN",   0x0006);

	SetConstant(proto, "ARRAY_BUFFER",                 0x8892);
	SetConstant(proto, "ELEMENT_ARRAY_BUFFER",         0x8893);
	SetConstant(proto, "ARRAY_BUFFER_BINDING",         0x8894);
	SetConstant(proto, "ELEMENT_ARRAY_BUFFER_BINDING", 0x8895);

        SetConstant(proto, "STREAM_DRAW",  0x88E0);
	SetConstant(proto, "STATIC_DRAW",  0x88E4);
	SetConstant(proto, "DYNAMIC_DRAW", 0x88E8);
		
        SetConstant(proto, "FRONT",          0x0404);
        SetConstant(proto, "BACK",           0x0405);
        SetConstant(proto, "FRONT_AND_BACK", 0x0408);
 
        SetConstant(proto, "BYTE",           0x1400);
        SetConstant(proto, "UNSIGNED_BYTE",  0x1401);
        SetConstant(proto, "SHORT",          0x1402);
        SetConstant(proto, "UNSIGNED_SHORT", 0x1403);
        SetConstant(proto, "INT",            0x1404);
        SetConstant(proto, "UNSIGNED_INT",   0x1405);
        SetConstant(proto, "FLOAT",          0x1406);

	SetConstant(proto, "FRAGMENT_SHADER", 0x8B30);
        SetConstant(proto, "VERTEX_SHADER",   0x8B31);

	SetConstant(proto, "NEAREST", 0x2600);
	SetConstant(proto, "LINEAR",  0x2601);

	SetConstant(proto, "TEXTURE_MAG_FILTER", 0x2800);
	SetConstant(proto, "TEXTURE_MIN_FILTER", 0x2801);
	SetConstant(proto, "TEXTURE_WRAP_S",     0x2802);
	SetConstant(proto, "TEXTURE_WRAP_T",     0x2803);

	SetConstant(proto, "TEXTURE_2D", 0x0DE1);
	SetConstant(proto, "TEXTURE",    0x1702);

	SetConstant(proto, "REPEAT",          0x2901);
	SetConstant(proto, "CLAMP_TO_EDGE",   0x812F);
	SetConstant(proto, "MIRRORED_REPEAT", 0x8370);
	
	// Methods
        NODE_SET_PROTOTYPE_METHOD(t, "viewport", Viewport);

        NODE_SET_PROTOTYPE_METHOD(t, "createBuffer", CreateBuffer);
        NODE_SET_PROTOTYPE_METHOD(t, "bindBuffer", BindBuffer);
        NODE_SET_PROTOTYPE_METHOD(t, "bufferData", BufferData);

        NODE_SET_PROTOTYPE_METHOD(t, "vertexAttribPointer", VertexAttribPointer);
        NODE_SET_PROTOTYPE_METHOD(t, "getAttribLocation", GetAttribLocation);
        NODE_SET_PROTOTYPE_METHOD(t, "bindAttribLocation", BindAttribLocation);
        NODE_SET_PROTOTYPE_METHOD(t, "enableVertexAttribArray", EnableVertexAttribArray);

        NODE_SET_PROTOTYPE_METHOD(t, "getUniformLocation", GetUniformLocation);
        NODE_SET_PROTOTYPE_METHOD(t, "uniform1i", Uniform1i);
        NODE_SET_PROTOTYPE_METHOD(t, "uniformMatrix4fv", UniformMatrix4fv);

        NODE_SET_PROTOTYPE_METHOD(t, "texParameterf", TexParameterf);
        NODE_SET_PROTOTYPE_METHOD(t, "texParameteri", TexParameteri);

        NODE_SET_PROTOTYPE_METHOD(t, "createShader", CreateShader);
        NODE_SET_PROTOTYPE_METHOD(t, "attachShader", AttachShader);
        NODE_SET_PROTOTYPE_METHOD(t, "shaderSource", ShaderSource);
        NODE_SET_PROTOTYPE_METHOD(t, "compileShader", CompileShader);

        NODE_SET_PROTOTYPE_METHOD(t, "createProgram", CreateProgram);
        NODE_SET_PROTOTYPE_METHOD(t, "linkProgram", LinkProgram);
        NODE_SET_PROTOTYPE_METHOD(t, "useProgram", UseProgram);

        NODE_SET_PROTOTYPE_METHOD(t, "clearColor", ClearColor);
        NODE_SET_PROTOTYPE_METHOD(t, "clear", Clear);

        NODE_SET_PROTOTYPE_METHOD(t, "blendFunc", BlendFunc);
        NODE_SET_PROTOTYPE_METHOD(t, "enable", Enable);

        NODE_SET_PROTOTYPE_METHOD(t, "drawArrays", DrawArrays);

        NODE_SET_PROTOTYPE_METHOD(t, "createTexture", CreateTexture);
        NODE_SET_PROTOTYPE_METHOD(t, "bindTexture", BindTexture);
        NODE_SET_PROTOTYPE_METHOD(t, "texImage2D", TexImage2D);
        NODE_SET_PROTOTYPE_METHOD(t, "activeTexture", ActiveTexture);

        NODE_SET_PROTOTYPE_METHOD(t, "swapBuffers", SwapBuffers);

        target->Set(String::NewSymbol("GLContext"), t->GetFunction());
    }

    static Handle<Object>
    NewInstance () {
	    return constructor_template->GetFunction()->NewInstance();
    }

protected:

    static Handle<Value>
    New (const Arguments& args) {
        HandleScope scope;

	GLContext *context = new GLContext();
        context->Wrap(args.This());

        return args.This();
    }

    static Handle<Value>
    Viewport (const Arguments& args) {
        HandleScope scope;

	GLint x = args[0]->IntegerValue();
	GLint y = args[1]->IntegerValue();
	GLsizei width = args[2]->IntegerValue();
	GLsizei height = args[3]->IntegerValue();
	glViewport(x, y, width, height);

        return Undefined();
    }

    static Handle<Value>
    CreateBuffer (const Arguments& args) {
        HandleScope scope;

        GLuint o;
	glGenBuffers(1, &o);

        return scope.Close(Integer::New(o));
    }

    static Handle<Value>
    BindBuffer (const Arguments& args) {
        HandleScope scope;

	GLenum target = args[0]->Uint32Value();
	GLuint buffer = args[1]->Uint32Value();
	glBindBuffer(target, buffer);

        return Undefined();
    }

    static Handle<Value>
    BufferData (const Arguments& args) {
        HandleScope scope;

        Float32Array *fa = Unwrap<Float32Array>(args[1]->ToObject());

	GLenum target = args[0]->Uint32Value();
        GLsizei size = fa->Length();
        const GLvoid* data = fa->GetData();
	GLenum usage = args[2]->Uint32Value();

	glBufferData(target, size, data, usage);

        return Undefined();
    }

    static Handle<Value>
    VertexAttribPointer (const Arguments& args) {
        HandleScope scope;

	GLuint index = args[0]->Uint32Value();
	GLint size = args[1]->IntegerValue();
	GLenum type = args[2]->Uint32Value();
	GLboolean normalized = args[3]->BooleanValue();
	GLsizei stride = args[4]->IntegerValue();
	const GLvoid *offset = reinterpret_cast<void*>(static_cast<intptr_t>(args[5]->Uint32Value()));
	glVertexAttribPointer(index, size, type, normalized, stride, offset);

        return Undefined();
    }

    static Handle<Value>
    GetAttribLocation (const Arguments& args) {
	HandleScope scope;

	GLuint program = args[0]->Uint32Value();
	String::Utf8Value name_s(args[1]->ToString());
        const GLchar *name = *name_s;
	GLint location = glGetAttribLocation(program, name);

	return scope.Close(Integer::New(location));
    }

    static Handle<Value>
    BindAttribLocation (const Arguments& args) {
	HandleScope scope;

	GLuint program = args[0]->Uint32Value();
	GLuint index = args[1]->Uint32Value();
	String::Utf8Value name_s(args[2]->ToString());
        const GLchar *name = *name_s;
	glBindAttribLocation(program, index, name);

	return Undefined();
    }

    static Handle<Value>
    EnableVertexAttribArray (const Arguments &args) {
	HandleScope scope;

	GLuint index = args[0]->Uint32Value();
	glEnableVertexAttribArray(index);

	return Undefined();
    }

    static Handle<Value>
    GetUniformLocation (const Arguments& args) {
	HandleScope scope;

	GLuint program = args[0]->Uint32Value();
	String::Utf8Value name_s(args[1]->ToString());
        const GLchar *name = *name_s;
	glGetUniformLocation(program, name);

	return Undefined();
    }

    static Handle<Value>
    Uniform1i (const Arguments &args) {
	HandleScope scope;

	GLint location = args[0]->IntegerValue();
	GLint v0 = args[1]->IntegerValue();
	glUniform1i(location, v0);

	return Undefined();
    }

    static Handle<Value>
    UniformMatrix4fv (const Arguments &args) {
	HandleScope scope;

        Float32Array *fa = Unwrap<Float32Array>(args[2]->ToObject());

	GLint location = args[0]->IntegerValue();
	GLsizei count = fa->Length() / sizeof(float);
	GLboolean transpose = args[1]->BooleanValue();
	const GLfloat *value = (float *)fa->GetData();
	glUniformMatrix4fv(location, count, transpose, value);

	return Undefined();
    }

    static Handle<Value>
    TexParameterf (const Arguments& args) {
        HandleScope scope;

	GLenum target = args[0]->Uint32Value();
	GLenum pname = args[0]->Uint32Value();
	GLfloat param = args[2]->NumberValue();
	glTexParameterf(target, pname, param);

        return Undefined();
    }

    static Handle<Value>
    TexParameteri (const Arguments& args) {
        HandleScope scope;

	GLenum target = args[0]->Uint32Value();
	GLenum pname = args[0]->Uint32Value();
	GLint param = args[2]->Int32Value();
	glTexParameteri(target, pname, param);

        return Undefined();
    }

    static Handle<Value>
    CreateShader (const Arguments& args) {
        HandleScope scope;

	GLenum type = args[0]->Uint32Value();
	GLuint shader = glCreateShader(type);

        return scope.Close(Integer::New(shader));
    }

    static Handle<Value>
    AttachShader (const Arguments& args) {
        HandleScope scope;

	GLuint program = args[0]->Uint32Value();
	GLuint shader = args[1]->Uint32Value();
	glAttachShader(program, shader);

        return Undefined();
    }

    static Handle<Value>
    ShaderSource (const Arguments& args) {
        HandleScope scope;

	GLuint shader = args[0]->Uint32Value();
	String::Utf8Value source_s(args[1]->ToString());
	const GLchar *source = *source_s;
	GLint length = source_s.length();
	glShaderSource(shader, 1, &source, &length);

        return Undefined();
    }

    static Handle<Value>
    CompileShader (const Arguments& args) {
        HandleScope scope;

	GLuint shader = args[0]->Uint32Value();
	glCompileShader(shader);

        return Undefined();
    }

    static Handle<Value>
    CreateProgram (const Arguments& args) {
        HandleScope scope;

        GLuint program = glCreateProgram();

        return scope.Close(Integer::New(program));
     }

    static Handle<Value>
    LinkProgram (const Arguments& args) {
        HandleScope scope;

        GLuint shader = args[0]->Uint32Value();
        glLinkProgram(shader);

        return Undefined();
     }

    static Handle<Value>
    UseProgram (const Arguments& args) {
        HandleScope scope;

        GLuint shader = args[0]->Uint32Value();
        glUseProgram(shader);

        return Undefined();
    }

    static Handle<Value>
    ClearColor (const Arguments& args) {
        HandleScope scope;

	GLclampf r = args[0]->NumberValue();
	GLclampf g = args[1]->NumberValue();
	GLclampf b = args[2]->NumberValue();
	GLclampf a = args[3]->NumberValue();
	glClearColor(r, g, b, a);

        return Undefined();
    }

    static Handle<Value>
    BlendFunc (const Arguments& args) {
        HandleScope scope;

	GLenum sfactor = args[0]->Uint32Value();
	GLenum dfactor = args[0]->Uint32Value();
	glBlendFunc(sfactor, dfactor);

        return Undefined();
    }

    static Handle<Value>
    Enable (const Arguments& args) {
        HandleScope scope;

	GLenum cap = args[0]->Uint32Value();
	glEnable(cap);

        return Undefined();
    }

    static Handle<Value>
    Clear (const Arguments& args) {
        HandleScope scope;

	GLbitfield mask = args[0]->Int32Value();
	glClear(mask);

        return Undefined();
    }

    static Handle<Value>
    DrawArrays (const Arguments& args) {
	HandleScope scope;
	
	GLenum mode = args[0]->Uint32Value();
	GLint first = args[1]->Int32Value();
	GLsizei count = args[2]->IntegerValue();
	glDrawArrays(mode, first, count);

	return Undefined();
    }

    static Handle<Value>
    CreateTexture (const Arguments& args) {
	HandleScope scope;

        GLuint o;
	glGenTextures(1, &o);

        return scope.Close(Integer::New(o));
    }

    static Handle<Value>
    BindTexture (const Arguments& args) {
	HandleScope scope;

	GLenum target = args[0]->Uint32Value();
	GLuint texture = args[1]->Uint32Value();
	glBindTexture(target, texture);

        return Undefined();
    }


    static Handle<Value>
    ActiveTexture (const Arguments& args) {
	HandleScope scope;

	GLenum texture = args[0]->Uint32Value();
	glActiveTexture(texture);

        return Undefined();
    }

    static Handle<Value>
    TexImage2D (const Arguments& args) {
	HandleScope scope;

	GLenum target = args[0]->Uint32Value();
	GLint level = args[1]->IntegerValue();
	GLenum internalformat = args[2]->IntegerValue();
	GLenum format = args[3]->IntegerValue();
	GLenum type = args[4]->IntegerValue();
	Image *image = Unwrap<Image>(args[5]->ToObject());
	
	glTexImage2D(target, level, internalformat, image->GetWidth(),
		     image->GetHeight(), 0, format,
		     type, image->GetData());

	// We have to 
	glGenerateMipmapEXT(GL_TEXTURE_2D);

	return Undefined();
    }

    static Handle<Value>
    SwapBuffers (const Arguments& args) {
        HandleScope scope;

	SDL_GL_SwapBuffers();

        return Undefined();
    }

    static void
    SetConstant (Handle<ObjectTemplate> proto, const char *string, unsigned long value) {
        proto->Set(String::New(string), Integer::New(value), ReadOnly);
    }

private:

    static Persistent<FunctionTemplate> constructor_template;

};

Persistent<FunctionTemplate> GLContext::constructor_template;
