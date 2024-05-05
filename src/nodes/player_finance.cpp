#include "player_finance.h"

#include "./base_resource.h"

#ifdef CL_TRADING_DEBUG
MAKE_LOG(FINANCELOG, PlayerFinance)
#endif

const char *godot::CL::PlayerFinance::SGoldChanged{"gold-changed"};

godot::CL::PlayerFinance::PlayerFinance()
    : debug_(false),
      start_gold_(0),
      gold_(0),
      total_profits_(0),
      player_name_(""),
      income_history_(Dictionary()),
      expense_history_(Dictionary()) {}

godot::CL::PlayerFinance::~PlayerFinance() {}

void godot::CL::PlayerFinance::change_gold_(int amount) {
    gold_ += amount;
    emit_signal(SGoldChanged, gold_);
}

godot::CL::FinanceItem godot::CL::PlayerFinance::make_finance_item_(
    FinanceMainKind fm_kind, FinanceKind fs_kind, StringName name, int value,
    int amount, int ctx) {
    FinanceItem dict{};
    dict["main-kind"] = fm_kind;
    dict["sub-kind"] = fs_kind;
    dict["name"] = name;
    dict["value"] = value;
    dict["ctx"] = ctx;
    dict["amount"] = amount;
    return dict;
}

void godot::CL::PlayerFinance::add_to_total_history_(int total_value,
                                                     FinanceKind kind,
                                                     int ctx) {
    if (ctx > -1) {
        total_history_[ctx] =
            static_cast<int>(total_history_[ctx]) + total_value;
    } else {
        total_history_[kind] =
            static_cast<int>(total_history_[kind]) + total_value;
    }
    total_profits_ += total_value;
}

void godot::CL::PlayerFinance::initialize_finance(StringName player_name) {
    for (int i = RESOURCE_PASSENGER; i <= RESOURCE_MEDICINE; i++) {
        total_history_[i] = 0;
    }
    for (int i = FINANCE_SUB_KIND_RESOURCE; i <= FINANCE_SUB_KIND_STOCK_SELL;
         i++) {
        if (i > FINANCE_SUB_KIND_RESOURCE) {
            total_history_[i] = 0;
        }
        income_history_[i] = TypedArray<FinanceItem>();
        expense_history_[i] = TypedArray<FinanceItem>();
    }
    gold_ = start_gold_;
    player_name_ = player_name;
}

void godot::CL::PlayerFinance::add_to_finance_(FinanceMainKind fm_kind,
                                               FinanceKind fs_kind,
                                               StringName name, int value,
                                               int amount, int ctx) {
    Dictionary dict;
    if (fm_kind == FINANCE_MAIN_KIND_INCOME) {
        dict = income_history_;
    } else {
        dict = expense_history_;
    }
    PUSH_ASSIGN(TypedArray<FinanceItem>, dict, fs_kind,
                make_finance_item_(fm_kind, fs_kind, name, value, amount, ctx))
    int total_value{value * amount};
    if (fm_kind == FINANCE_MAIN_KIND_EXPENSE) {
        total_value *= -1;
    }
    int old_gold{gold_};
    change_gold_(total_value);
    add_to_total_history_(total_value, fs_kind, ctx);
#ifdef CL_TRADING_DEBUG
    FINANCELOG(
        this, "%s\n",
        GDSTR(vformat(
            "added fm=%d,fs=%d,name=%s,value=%d,amount=%d,ctx=%d,ng=%d,og=%d",
            fm_kind, fs_kind, name, value, amount, ctx, gold_, old_gold)));
#endif
}

void godot::CL::PlayerFinance::add_income(FinanceKind fs_kind, StringName name,
                                          int value, int amount, int ctx) {
    add_to_finance_(FINANCE_MAIN_KIND_INCOME, fs_kind, name, value, amount);
}

void godot::CL::PlayerFinance::add_expense(FinanceKind fs_kind, StringName name,
                                           int value, int amount, int ctx) {
    add_to_finance_(FINANCE_MAIN_KIND_EXPENSE, fs_kind, name, value, amount);
}

void godot::CL::PlayerFinance::_bind_methods() {
    DEBUG_BIND(PlayerFinance)

    ClassDB::bind_method(D_METHOD("get_start_gold"),
                         &PlayerFinance::get_start_gold);
    ClassDB::bind_method(D_METHOD("set_start_gold", "g"),
                         &PlayerFinance::set_start_gold);

    ClassDB::bind_method(D_METHOD("get_gold_"), &PlayerFinance::get_gold_);
    ClassDB::bind_method(D_METHOD("set_gold_", "g"), &PlayerFinance::set_gold_);

    ClassDB::add_property(
        "PlayerFinance",
        PropertyInfo(Variant::INT, "gold", PROPERTY_HINT_RANGE, "0,1000"),
        "set_start_gold", "get_start_gold");

    ClassDB::add_signal(
        "PlayerFinance",
        MethodInfo(SGoldChanged, PropertyInfo(Variant::INT, "new_amount")));

    BIND_ENUM_CONSTANT(FINANCE_MAIN_KIND_INCOME);
    BIND_ENUM_CONSTANT(FINANCE_MAIN_KIND_EXPENSE);

    BIND_ENUM_CONSTANT(FINANCE_SUB_KIND_RESOURCE);
    BIND_ENUM_CONSTANT(FINANCE_SUB_KIND_INDUSTRY);
    BIND_ENUM_CONSTANT(FINANCE_SUB_KIND_ROUTE);
    BIND_ENUM_CONSTANT(FINANCE_SUB_KIND_UPKEEP);
    BIND_ENUM_CONSTANT(FINANCE_SUB_KIND_UPGRADE);
    BIND_ENUM_CONSTANT(FINANCE_SUB_KIND_RECOUP);
    BIND_ENUM_CONSTANT(FINANCE_SUB_KIND_STOCK_BUY);
    BIND_ENUM_CONSTANT(FINANCE_SUB_KIND_STOCK_SELL);
}
