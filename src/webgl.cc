#include <SDL/SDL.h>

#include <v8.h>

#include <node.h>
#include <node_events.h>

#include "glcontext.h"
#include "typedarray.h"
#include "image.h"

using namespace v8;
using namespace node;

// How often to poll for events
#define WINDOW_EVENT_TIMEOUT 0.01
#define WINDOW_DEFAULT_WIDTH 640
#define WINDOW_DEFAULT_HEIGHT 480

class Window : public EventEmitter {
public:

    static void
    Initialize (Handle<Object> target) {
	HandleScope scope;

        Local<FunctionTemplate> t = FunctionTemplate::New(New);

        t->Inherit(EventEmitter::constructor_template);
        t->InstanceTemplate()->SetInternalFieldCount(1);

        NODE_SET_PROTOTYPE_METHOD(t, "getContext", GetContext);
        NODE_SET_PROTOTYPE_METHOD(t, "addEventListener", AddEventListener);
        NODE_SET_PROTOTYPE_METHOD(t, "close", Close);

	t->PrototypeTemplate()->SetAccessor(String::New("width"), WidthGetter);
	t->PrototypeTemplate()->SetAccessor(String::New("height"), HeightGetter);

        target->Set(String::NewSymbol("Window"), t->GetFunction());
    }

protected:

    static Handle<Value>
    New (const Arguments& args) {
        HandleScope scope;

	int width = WINDOW_DEFAULT_WIDTH;
	if (args.Length() >= 1) {
	    width = args[0]->IntegerValue();
	}

	int height = WINDOW_DEFAULT_HEIGHT;
	if (args.Length() >= 2) {
	    height = args[1]->IntegerValue();
	}

	window = new Window(width, height);
        window->Wrap(args.This());

        return args.This();
    }

    static Handle<Value>
    GetContext (const Arguments& args) {
        HandleScope scope;

	Handle<Object> context = GLContext::NewInstance();

        return scope.Close(context);
    }

    static Handle<Value>
    AddEventListener (const Arguments& args) {
        HandleScope scope;

	// Fallback to addListener

	Local<Value> f_v = args.This()->Get(String::New("addListener"));
	if (!f_v->IsFunction()) return Undefined();

	Local<Function> f = Local<Function>::Cast(f_v);

	Handle<Value> values[args.Length()];
	for (int i = 0; i < args.Length(); i++) {
	    values[i] = args[i];
	}
	
	return f->Call(args.This(), args.Length(), values);
    }

    static Handle<Value>
    Close (const Arguments& args) {
        HandleScope scope;

	delete window;
	window = NULL;

        return Undefined();
    }

    static Handle<Value>
    WidthGetter (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;

	SDL_Surface *surface = SDL_GetVideoSurface();

	return scope.Close(Integer::New(surface->w));
    }

    static Handle<Value>
    HeightGetter (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;

	SDL_Surface *surface = SDL_GetVideoSurface();

	return scope.Close(Integer::New(surface->h));
    }

    Window (int width, int height) : EventEmitter() {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_SetVideoMode(width, height, 32, SDL_OPENGL);

	ev_timer_init(&timeout_watcher, OnTimeout, WINDOW_EVENT_TIMEOUT, 0.0);
	ev_timer_start(EV_DEFAULT_UC_ &timeout_watcher);
    }

    ~Window () {
	ev_timer_stop(&timeout_watcher);
	SDL_Quit ();
    }

    static void
    OnTimeout (EV_P_ ev_timer *w, int revents) {
	SDL_Event event;

	if (window == NULL) {
	    return;
	}

	while (SDL_PollEvent(&event)) {
	    switch (event.type) {
	    case SDL_QUIT:
		window->Emit(String::New("quit"), 0, NULL);
		break;
	    case SDL_MOUSEMOTION: {
		Handle<Object> object = Object::New();
		object->Set(String::New("offsetX"), Integer::New(event.motion.x));
		object->Set(String::New("offsetY"), Integer::New(event.motion.y));

		Handle<Value> args[1];
		args[0] = object;

		window->Emit(String::New("mousemove"), 1, args);
		break;
	    }
	    case SDL_APPMOUSEFOCUS:
		if (event.active.state == SDL_APPMOUSEFOCUS) {
		    if (event.active.gain) {
			window->Emit(String::New("mouseover"), 0, NULL);
		    } else {
			window->Emit(String::New("mouseout"), 0, NULL);
		    }
		}
		break;
	    case SDL_MOUSEBUTTONDOWN:
		window->Emit(String::New("mousedown"), 0, NULL);
		break;
	    case SDL_MOUSEBUTTONUP:
		window->Emit(String::New("mouseup"), 0, NULL);
		break;
	    }
	}

	ev_timer_start(EV_DEFAULT_UC_ w);
    }

private:

    static Window *window;
    ev_timer timeout_watcher;

};

Window *Window::window;

extern "C" void
init (Handle<Object> target) {
    HandleScope scope;

    GLContext::Initialize(target);
    Float32Array::Initialize(target);
    Image::Initialize(target);
    Window::Initialize(target);
}
