#ifndef IMAGE_H_
#define IMAGE_H_

#include <v8.h>

#include <node.h>

#include <IL/il.h>

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

	ilInit();
    }

    void
    FlipVertically () {
	int width = GetWidth();
	int height = GetHeight();

	int rowBytes = width * 3;
	uint8_t* tempRow = new uint8_t[rowBytes];
	for (unsigned i = 0; i < height / 2; i++) {
	    uint8_t* lowRow = (uint8_t *)data + (rowBytes * i);
	    uint8_t* highRow = (uint8_t *)data + (rowBytes * (height - i - 1));
	    memcpy(tempRow, lowRow, rowBytes);
	    memcpy(lowRow, highRow, rowBytes);
	    memcpy(highRow, tempRow, rowBytes);
	}
	delete[] tempRow;
    }

    int
    GetBPP () {
	ilBindImage(image_id);
	return ilGetInteger(IL_IMAGE_BPP);
    }

    int
    GetWidth () {
	ilBindImage(image_id);
	return ilGetInteger(IL_IMAGE_WIDTH);
    }

    int
    GetHeight () {
	ilBindImage(image_id);
	return ilGetInteger(IL_IMAGE_HEIGHT);
    }

    int
    GetFormat () {
	ilBindImage(image_id);
	return ilGetInteger(IL_IMAGE_FORMAT);
    }

    void *
    GetData () {
	return data;
    }

    void
    Load (const char *filename) {
	this->filename = (char *)filename;

	ilBindImage(image_id);
	ilLoadImage(filename);
	ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

	data = ilGetData();
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
	ilGenImages(1, &image_id);
    }

    ~Image () {
	ilDeleteImages(1, &image_id);
    }

private:

    ILuint image_id;
    char *filename;
    void *data;

};

#endif  // IMAGE_H_
