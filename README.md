node-webgl
==========

Screenshot
----------

![](http://brianmckenna.org/blog/static/nodejs_webgl.png)

Dependencies
------------

* node.js
* SDL
* OpenGL
* FreeImage

Building
--------

    node-waf configure build

Testing
-------

    node examples/example.js
    chromium-browser examples/example.html

On Mac OS X, you have to run the following first:

    export SDL_VIDEODRIVER=x11

Developing
----------

* Add more WebGL functions!
* Make some node.js image libraries
* Find nice ways to remove the `nodejs` variable from example-include.js
* Add more events (e.g. keyboard)
* Make/port some WebGL examples

See
---

* [My quick blog post](http://brianmckenna.org/blog/nodejs_webgl)
