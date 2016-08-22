#include "nitrogen/nitrogen.hpp"

#include <node.h>

#include <v8pp/module.hpp>
#include <v8pp/class.hpp>

static void init(v8::Handle<v8::Object> exports, v8::Handle<v8::Object> module)
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	v8pp::module nitrogen_module(isolate);

	v8pp::class_<event_emitter> event_emitter_class(isolate);
	event_emitter_class
		.set("on", &event_emitter::on)
		.set("off", &event_emitter::off)
		;
	nitrogen_module.set("EventEmitter", event_emitter_class);

	exports->SetPrototype(nitrogen_module.new_instance());
}

NODE_MODULE(nitrogen, init)
