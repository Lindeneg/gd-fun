#include "industry.h"

const char *godot::CL::Industry::SResourceProcessStarted{"process-started"};
const char *godot::CL::Industry::SResourceProcessFinished{"process-finished"};

godot::CL::Industry::Industry()
    : total_profits_(0),
      timer_(nullptr),
      owner_(""),
      debug_(false),
      state_(INDUSTRY_STATE_IDLE),
      in_kind_(RESOURCE_PASSENGER),
      out_kind_(RESOURCE_PASSENGER),
      time_to_convert_(0.0f),
      conversion_amount_(0),
      in_amount_(0),
      out_amount_(0) {}

godot::CL::Industry::~Industry() {
    Utils::queue_delete(timer_);
    timer_ = nullptr;
}

void godot::CL::Industry::on_process_resource_finished_() {
    ERR_FAIL_COND(state_ != INDUSTRY_STATE_PROCESSING);
    out_amount_ += 1;
    state_ = INDUSTRY_STATE_IDLE;
    emit_signal(SResourceProcessFinished, out_kind_, out_amount_);
    if (in_amount_ >= conversion_amount_) {
        queue_process_resource_();
    }
}

void godot::CL::Industry::queue_process_resource_() {
    if (state_ == INDUSTRY_STATE_PROCESSING ||
        in_amount_ < conversion_amount_) {
        return;
    }
    state_ = INDUSTRY_STATE_PROCESSING;
    in_amount_ -= conversion_amount_;
    emit_signal(SResourceProcessStarted, in_kind_, in_amount_);
    timer_->start();
}

godot::Timer *godot::CL::Industry::initialize_timer() {
    if (has_timer() || Utils::is_in_editor()) return nullptr;
    timer_ = memnew(Timer);
    timer_->set_wait_time(time_to_convert_);
    timer_->set_one_shot(true);
    timer_->connect("timeout", Callable(this, "on_process_resource_finished_"));
    return timer_;
}

void godot::CL::Industry::_bind_methods() {
    // BIND METHODS
    DEBUG_BIND(Industry)

    ClassDB::bind_method(D_METHOD("get_owner"), &Industry::get_owner);
    ClassDB::bind_method(D_METHOD("set_owner", "o"), &Industry::set_owner);

    ClassDB::bind_method(D_METHOD("on_process_resource_finished_"),
                         &Industry::on_process_resource_finished_);
    ClassDB::bind_method(D_METHOD("queue_process_resource_"),
                         &Industry::queue_process_resource_);

    ClassDB::bind_method(D_METHOD("get_in_kind"), &Industry::get_in_kind);
    ClassDB::bind_method(D_METHOD("get_out_kind"), &Industry::get_out_kind);
    ClassDB::bind_method(D_METHOD("get_conversion_amount"),
                         &Industry::get_conversion_amount);
    ClassDB::bind_method(D_METHOD("set_conversion_amount", "c"),
                         &Industry::set_conversion_amount);
    ClassDB::bind_method(D_METHOD("set_in_kind", "k"), &Industry::set_in_kind);
    ClassDB::bind_method(D_METHOD("set_out_kind", "k"),
                         &Industry::set_out_kind);

    ClassDB::bind_method(D_METHOD("get_state"), &Industry::get_state);

    ClassDB::bind_method(D_METHOD("get_in_amount"), &Industry::get_in_amount);
    ClassDB::bind_method(D_METHOD("set_in_amount", "a"),
                         &Industry::set_in_amount);
    ClassDB::bind_method(D_METHOD("get_out_amount"), &Industry::get_out_amount);
    ClassDB::bind_method(D_METHOD("set_out_amount", "a"),
                         &Industry::set_out_amount);

    ClassDB::bind_method(D_METHOD("get_time_to_convert"),
                         &Industry::get_time_to_convert);
    ClassDB::bind_method(D_METHOD("set_time_to_convert", "t"),
                         &Industry::set_time_to_convert);

    ClassDB::add_property("Industry", BaseResource::get_kind_prop_info("in"),
                          "set_in_kind", "get_in_kind");
    ClassDB::add_property("Industry", BaseResource::get_kind_prop_info("out"),
                          "set_out_kind", "get_out_kind");
    ClassDB::add_property("Industry", PropertyInfo(Variant::INT, "in_amount"),
                          "set_in_amount", "get_in_amount");
    ClassDB::add_property("Industry", PropertyInfo(Variant::INT, "out_amount"),
                          "set_out_amount", "get_out_amount");
    ClassDB::add_property("Industry", PropertyInfo(Variant::INT, "conversion"),
                          "set_conversion_amount", "get_conversion_amount");
    ClassDB::add_property("Industry",
                          PropertyInfo(Variant::FLOAT, "time_to_convert"),
                          "set_time_to_convert", "get_time_to_convert");
    ClassDB::add_property("Industry",
                          PropertyInfo(Variant::STRING_NAME, "owner"),
                          "set_owner", "get_owner");

    BIND_ENUM_CONSTANT(INDUSTRY_STATE_IDLE);
    BIND_ENUM_CONSTANT(INDUSTRY_STATE_PROCESSING);

    ClassDB::add_signal(
        "Industry",
        MethodInfo(SResourceProcessStarted, PropertyInfo(Variant::INT, "kind"),
                   PropertyInfo(Variant::INT, "amount")));
    ClassDB::add_signal(
        "Industry",
        MethodInfo(SResourceProcessFinished, PropertyInfo(Variant::INT, "kind"),
                   PropertyInfo(Variant::INT, "amount")));
}
