// MIT License
// 
// Copyright (c) 2022 Aleksey Loginov
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "rpp/utils/function_traits.h"

#include <rpp/observers/interface_observer.h>
#include <rpp/utils/functors.h>

#include <exception>

namespace rpp::details
{
template<typename T,
         typename State,
         typename OnNext = utils::empty_function_t<T, State>,
         typename OnError = utils::empty_function_t<std::exception_ptr, State>,
         typename OnCompleted = utils::empty_function_t<State>>
class state_observer final : public interface_observer<T>
{
public:
    template<typename TState,
             typename TOnNext = utils::empty_function_t<T, State>,
             typename TOnError = utils::empty_function_t<std::exception_ptr, State>,
             typename TOnCompleted = utils::empty_function_t<State>,
             typename = std::enable_if_t<std::is_invocable_v<TOnNext, T, State> && std::is_invocable_v<TOnError, std::exception_ptr, State> && std::is_invocable_v<TOnCompleted, State>>>
    state_observer(TState&& state, TOnNext&& on_next = {}, TOnError&& on_error = {}, TOnCompleted&& on_completed = {})
        : m_state{std::forward<TState>(state)}
        , m_on_next{std::forward<TOnNext>(on_next)}
        , m_on_err{std::forward<TOnError>(on_error)}
        , m_on_completed{std::forward<TOnCompleted>(on_completed)} {}

    state_observer(const state_observer<T, State, OnNext, OnError, OnCompleted>& other)     = default;
    state_observer(state_observer<T, State, OnNext, OnError, OnCompleted>&& other) noexcept = default;

    void on_next(const T& v) const override                     { m_on_next(v, m_state);             }
    void on_next(T&& v) const override                          { m_on_next(std::move(v), m_state);  }
    void on_error(const std::exception_ptr& err) const override { m_on_err(err, m_state);            }
    void on_completed() const override                          { m_on_completed(m_state);         }

private:
    State       m_state;
    OnNext      m_on_next;
    OnError     m_on_err;
    OnCompleted m_on_completed;
};

template<typename TState, typename TOnNext, typename ...Args>
state_observer(TState, TOnNext, Args...) -> state_observer<std::decay_t<utils::function_argument_t<TOnNext>>, TState, TOnNext, Args...>;
} // namespace rpp::details