#pragma once

#include "function.h"

#include "context.h"
#include "eval.h"

namespace slip {
template <class R>
template <class F>
NormalFunc<R>::NormalFunc(std::string name, F&& f)
    : Function(name + ManglerCaller<std::decay_t<F>>::Mangle(),
               ManglerCaller<std::decay_t<F>>::Result(),
               FunctionType::Normal),
      fun_([f = std::move(f)](const Val& x, Context & ctx)->R {
          return FunApply(f,
                          x,
                          ctx,
                          std::make_index_sequence<
                              ManglerCaller<std::decay_t<F>>::arity>());
      }) {}

template <class R>
R Function::Call(const Val& x, Context& ctx) const {
    if (type_ == FunctionType::Normal) {
        return static_cast<const NormalFunc<R>&>(*this)(x, ctx);
    } else {
        return static_cast<const SpecialFunc&>(*this).Call<R>(x, ctx);
    }
}

template <class R>
template <class F, std::size_t... Ns>
R NormalFunc<R>::FunApply(F&& f,
                          const Val& x,
                          Context& ctx,
                          std::index_sequence<Ns...>) {
    // HAIL SATAN
    return f(Eval<std::decay_t<decltype(std::get<Ns>(
                 std::declval<
                     typename ManglerCaller<std::decay_t<F>>::args_type>()))>>(
        *dynamic_cast<const List&>(x)[Ns + 1], ctx)...);
}

}  // namespace slip
