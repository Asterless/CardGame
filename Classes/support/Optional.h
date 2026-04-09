#ifndef CARDGAME_SUPPORT_OPTIONAL_H
#define CARDGAME_SUPPORT_OPTIONAL_H

#include <utility>

namespace cardgame
{
    struct NullOptTag
    {
        explicit constexpr NullOptTag(int) noexcept
        {
        }
    };

    using nullopt_t = NullOptTag;

    constexpr NullOptTag nullopt{0};

    template <typename T>
    class Optional
    {
    public:
        Optional() = default;
        Optional(nullopt_t) noexcept
        {
        }

        Optional(const T &value)
            : _hasValue(true), _value(value)
        {
        }

        Optional(T &&value)
            : _hasValue(true), _value(std::move(value))
        {
        }

        Optional &operator=(nullopt_t) noexcept
        {
            _hasValue = false;
            _value = T{};
            return *this;
        }

        Optional &operator=(const T &value)
        {
            _hasValue = true;
            _value = value;
            return *this;
        }

        Optional &operator=(T &&value)
        {
            _hasValue = true;
            _value = std::move(value);
            return *this;
        }

        bool has_value() const noexcept
        {
            return _hasValue;
        }

        explicit operator bool() const noexcept
        {
            return _hasValue;
        }

        T &operator*() noexcept
        {
            return _value;
        }

        const T &operator*() const noexcept
        {
            return _value;
        }

        T *operator->() noexcept
        {
            return &_value;
        }

        const T *operator->() const noexcept
        {
            return &_value;
        }

    private:
        bool _hasValue = false;
        T _value{};
    };
}

#endif
