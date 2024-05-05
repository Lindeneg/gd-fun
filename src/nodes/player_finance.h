#ifndef CL_TRADING_PLAYER_FINANCE_H_
#define CL_TRADING_PLAYER_FINANCE_H_

#include <godot_cpp/classes/node.hpp>

#include "../core/utils.h"

namespace godot::CL {

enum FinanceMainKind { FINANCE_MAIN_KIND_INCOME, FINANCE_MAIN_KIND_EXPENSE };
enum FinanceKind {
    FINANCE_SUB_KIND_RESOURCE = 50,
    FINANCE_SUB_KIND_INDUSTRY = 51,
    FINANCE_SUB_KIND_ROUTE = 52,
    FINANCE_SUB_KIND_UPKEEP = 53,
    FINANCE_SUB_KIND_UPGRADE = 54,
    FINANCE_SUB_KIND_RECOUP = 55,
    FINANCE_SUB_KIND_STOCK_BUY = 56,
    FINANCE_SUB_KIND_STOCK_SELL = 57
};

typedef Dictionary FinanceItem;

class PlayerFinance : public Node {
    GDCLASS(PlayerFinance, Node)

   private:
    bool debug_;
    int start_gold_;
    int gold_;
    int total_profits_;
    StringName player_name_;
    Dictionary total_history_;
    Dictionary income_history_;
    Dictionary expense_history_;

    static FinanceItem make_finance_item_(FinanceMainKind fm_kind,
                                          FinanceKind fs_kind, StringName name,
                                          int value, int amount, int ctx);

    void add_to_finance_(FinanceMainKind fm_kind, FinanceKind fs_kind,
                         StringName name, int value, int amount, int ctx = -1);
    void add_to_total_history_(int total_value, FinanceKind kind, int ctx);
    void change_gold_(int amount);

    inline int get_gold_() const { return gold_; }
    inline void set_gold_(const int g) { gold_ = g; }

   protected:
    static void _bind_methods();

   public:
    PlayerFinance();
    ~PlayerFinance();

    const static char *SGoldChanged;

    DEBUG_METHODS()

    inline int get_start_gold() const { return start_gold_; }
    inline void set_start_gold(const int g) { start_gold_ = g; }

    void initialize_finance(StringName player_name);
    void add_income(FinanceKind fs_kind, StringName name, int value, int amount,
                    int ctx = -1);
    void add_expense(FinanceKind fs_kind, StringName name, int value,
                     int amount, int ctx = -1);
};
}  // namespace godot::CL

VARIANT_ENUM_CAST(godot::CL::FinanceMainKind);
VARIANT_ENUM_CAST(godot::CL::FinanceKind);

#endif  // CL_TRADING_PLAYER_FINANCE_H_
