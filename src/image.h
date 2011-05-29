#ifndef IMAGE_H_
#define IMAGE_H_

#include <v8.h>

#include <node.h>

#include <FreeImage.h>

using namespace v8;
using namespace node;

class Image : public EventEmitter {
public:

    static void
    Initialize (Handle<Object> target) {
	HandleScope scope;

	Local<FunctionTemplate> t = FunctionTemplate::New(New);

        t->Inherit(EventEmitter::constructor_template);
        t->InstanceTemplate()->SetInternalFieldCount(1);

	t->PrototypeTemplate()->SetAccessor(String::New("width"), WidthGetter);
	t->PrototypeTemplate()->SetAccessor(String::New("height"), HeightGetter);
	t->PrototypeTemplate()->SetAccessor(String::New("src"), SrcGetter, SrcSetter);

        NODE_SET_PROTOTYPE_METHOD(t, "addEventListener", AddEventListener);

	target->Set(String::NewSymbol("Image"), t->GetFunction());

	FreeImage_Initialise(true);
    }

    int
    GetWidth () {
	return FreeImage_GetWidth(image_bmp);
    }

    int
    GetHeight () {
	return FreeImage_GetHeight(image_bmp);
    }

    void *
    GetData () {
	BYTE *pixels = FreeImage_GetBits(image_bmp);

	// FreeImage stores data in BGR
	// Convert from BGR to RGB
	for(int i = 0; i < FreeImage_GetWidth(image_bmp) * FreeImage_GetHeight(image_bmp); i++)
	{
		BYTE temp = pixels[i * 4 + 0];
		pixels[i * 4 + 0] = pixels[i * 4 + 2];
		pixels[i * 4 + 2] = temp;
	}

	return pixels;
    }

    void
    Load (const char *filename) {
	this->filename = (char *)filename;

	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename, 0);
	image_bmp = FreeImage_Load(format, filename, 0);
	image_bmp = FreeImage_ConvertTo32Bits(image_bmp);
    }

protected:

    static Handle<Value>
    New (const Arguments& args) {
	HandleScope scope;


	Image *image = new Image();
	image->Wrap(args.This());

	return args.This();
    }

    static Handle<Value>
    WidthGetter (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;

	Image *image = ObjectWrap::Unwrap<Image>(info.This());

	return scope.Close(Integer::New(image->GetWidth()));
    }


    static Handle<Value>
    HeightGetter (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;

	Image *image = ObjectWrap::Unwrap<Image>(info.This());

	return scope.Close(Integer::New(image->GetHeight()));
    }


    static Handle<Value>
    SrcGetter (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;

	Image *image = ObjectWrap::Unwrap<Image>(info.This());

	return scope.Close(String::New(image->filename));
    }

    static void
    SrcSetter (Local<String> property, Local<Value> value, const AccessorInfo& info) {
	HandleScope scope;

	Image *image = ObjectWrap::Unwrap<Image>(info.This());
	String::Utf8Value filename_s(value->ToString());
	image->Load(*filename_s);

	image->Emit(String::New("load"), 0, NULL);
    }

    static Handle<Value>
    AddEventListener (const Arguments& args) {
        HandleScope scope;

	// Fallback to addListener

	Local<Value> f_v = args.This()->Get(String::New("addListener"));
	if (!f_v->IsFunction()) return Undefined();

	Local<Function> f = Local<Function>::Cast(f_v);

	Handle<Value> values[args.Length()];
	for(int i = 0; i < args.Length(); i++) {
	    values[i] = args[i];
	}
	
	return f->Call(args.This(), args.Length(), values);
    }

    Image () : EventEmitter() {
    }

    ~Image () {
	if (image_bmp) FreeImage_Unload(image_bmp);
	FreeImage_DeInitialise();
    }

private:

    FIBITMAP *image_bmp;
    char *filename;
    void *data;

};

#endif  // IMAGE_H_
