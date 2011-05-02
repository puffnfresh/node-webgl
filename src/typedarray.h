#ifndef TYPEDARRAYS_H_
#define TYPEDARRAYS_H_

#include <v8.h>

#include <node.h>
#include <node_buffer.h>

using namespace v8;
using namespace node;

class Float32Array : public ObjectWrap {
public:

    static void
    Initialize (Handle<Object> target) {
	HandleScope scope;

	Local<FunctionTemplate> t = FunctionTemplate::New(New);
	t->InstanceTemplate()->SetInternalFieldCount(1);
	t->SetClassName(String::NewSymbol("Float32Array"));

	NODE_SET_PROTOTYPE_METHOD(t, "set", Set);

	t->PrototypeTemplate()->SetAccessor(String::New("length"), LengthGetter);

	target->Set(String::NewSymbol("Float32Array"), t->GetFunction());
    }

    void *
    GetData () {
	Local<Value> buf = handle_->GetHiddenValue(String::New("buffer"));

	return (void *)Buffer::Data(buf->ToObject());
    }

    size_t
    Length () {
	Local<Value> buf = handle_->GetHiddenValue(String::New("buffer"));

	return Buffer::Length(buf->ToObject());
    }

    void
    Set (float *sequence, uint32_t length) {
	Local<Value> buf = handle_->GetHiddenValue(String::New("buffer"));

	float *data = (float *)Buffer::Data(buf->ToObject());
	for(int i = 0; i < length; i++) {
	    data[i] = sequence[i];
	}
    }

protected:

    static Handle<Value>
    New (const Arguments& args) {
	HandleScope scope;

	Float32Array *fa = new Float32Array();
	fa->Wrap(args.This());

	fa->handle_->SetHiddenValue(String::New("buffer"), args[0]);

	return args.This();
    }

    static Handle<Value>
    Set (const Arguments& args) {
	HandleScope scope;

        Float32Array *fa = Unwrap<Float32Array>(args.This());

	Local<Array> a = Local<Array>::Cast(args[0]);

	size_t length = a->Length();
	float *sequence = new float[length];
	for(int i = 0; i < length; i++) {
	    Local<Value> val = a->Get(i);
	    sequence[i] = val->NumberValue();
	}
	fa->Set(sequence, length);
	delete[] sequence;

	return Undefined();
    }

    static Handle<Value>
    LengthGetter (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;

        Float32Array *fa = Unwrap<Float32Array>(info.This());
	Local<Value> buf = fa->handle_->GetHiddenValue(String::New("buffer"));

	int len = Buffer::Length(buf->ToObject());

	return scope.Close(Integer::New(len));
    }

    Float32Array () : ObjectWrap() {
    }

};

#endif  // TYPEDARRAYS_H_
