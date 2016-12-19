#include "point-wrapper.h"
#include <cmath>
#include "nan.h"

using namespace v8;

static Nan::Persistent<String> row_string;
static Nan::Persistent<String> column_string;
static Nan::Persistent<v8::Function> constructor;

Nan::Maybe<Point> PointWrapper::PointFromJS(Nan::MaybeLocal<Object> maybe_object) {
  Local<Object> object;
  if (!maybe_object.ToLocal(&object)) {
    Nan::ThrowTypeError("Expected an object with 'row' and 'column' properties.");
    return Nan::Nothing<Point>();
  }

  Nan::MaybeLocal<Integer> maybe_row = Nan::To<Integer>(object->Get(Nan::New(row_string)));
  Local<Integer> js_row;
  if (!maybe_row.ToLocal(&js_row)) {
    Nan::ThrowTypeError("Expected an object with 'row' and 'column' properties.");
    return Nan::Nothing<Point>();
  }

  Nan::MaybeLocal<Integer> maybe_column = Nan::To<Integer>(object->Get(Nan::New(column_string)));
  Local<Integer> js_column;
  if (!maybe_column.ToLocal(&js_column)) {
    Nan::ThrowTypeError("Expected an object with 'row' and 'column' properties.");
    return Nan::Nothing<Point>();
  }

  unsigned row, column;
  if (std::isfinite(js_row->NumberValue())) {
    row = static_cast<unsigned>(js_row->Int32Value());
  } else {
    row = UINT_MAX;
  }

  if (std::isfinite(js_column->NumberValue())) {
    column = static_cast<unsigned>(js_column->Int32Value());
  } else {
    column = UINT_MAX;
  }

  return Nan::Just(Point(row, column));
}

void PointWrapper::Init() {
  row_string.Reset(Nan::Persistent<String>(Nan::New("row").ToLocalChecked()));
  column_string.Reset(Nan::Persistent<String>(Nan::New("column").ToLocalChecked()));

  Local<FunctionTemplate> constructor_template = Nan::New<FunctionTemplate>(New);
  constructor_template->SetClassName(Nan::New<String>("Point").ToLocalChecked());
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  Nan::SetAccessor(constructor_template->InstanceTemplate(), Nan::New(row_string), GetRow);
  Nan::SetAccessor(constructor_template->InstanceTemplate(), Nan::New(column_string), GetColumn);
  constructor.Reset(constructor_template->GetFunction());
}

Local<Value> PointWrapper::FromPoint(Point point) {
  Local<Object> result;
  if (Nan::New(constructor)->NewInstance(Nan::GetCurrentContext()).ToLocal(&result)) {
    (new PointWrapper(point))->Wrap(result);
    return result;
  } else {
    return Nan::Null();
  }
}

PointWrapper::PointWrapper(Point point) : point(point) {}

void PointWrapper::New(const Nan::FunctionCallbackInfo<Value> &info) {}

void PointWrapper::GetRow(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info) {
  PointWrapper *wrapper = Nan::ObjectWrap::Unwrap<PointWrapper>(info.This());
  Point &point = wrapper->point;
  info.GetReturnValue().Set(Nan::New(point.row));
}

void PointWrapper::GetColumn(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info) {
  PointWrapper *wrapper = Nan::ObjectWrap::Unwrap<PointWrapper>(info.This());
  Point &point = wrapper->point;
  info.GetReturnValue().Set(Nan::New(point.column));
}
