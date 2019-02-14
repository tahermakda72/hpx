//  Copyright (c) 2011 Thomas Heller
//  Copyright (c) 2013 Hartmut Kaiser
//  Copyright (c) 2014 Agustin Berge
//  Copyright (c) 2017 Google
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_UTIL_DETAIL_BASIC_FUNCTION_HPP
#define HPX_UTIL_DETAIL_BASIC_FUNCTION_HPP

#include <hpx/config.hpp>
#include <hpx/runtime/serialization/serialization_fwd.hpp>
#include <hpx/traits/get_function_address.hpp>
#include <hpx/traits/get_function_annotation.hpp>
#include <hpx/traits/is_callable.hpp>
#include <hpx/util/assert.hpp>
#include <hpx/util/detail/empty_function.hpp>
#include <hpx/util/detail/vtable/serializable_function_vtable.hpp>
#include <hpx/util/detail/vtable/serializable_vtable.hpp>
#include <hpx/util/detail/vtable/function_vtable.hpp>
#include <hpx/util/detail/vtable/unique_function_vtable.hpp>
#include <hpx/util/detail/vtable/vtable.hpp>

#include <cstddef>
#include <cstring>
#include <string>
#include <type_traits>
#include <utility>

namespace hpx { namespace util { namespace detail
{
    static const std::size_t function_storage_size = 3 * sizeof(void*);

    ///////////////////////////////////////////////////////////////////////////
    template <typename Sig, bool Copyable>
    class function_base;

    template <bool Copyable, typename R, typename ...Ts>
    class function_base<R(Ts...), Copyable>
    {
        using vtable = typename std::conditional<
                Copyable,
                detail::function_vtable<R(Ts...)>,
                detail::unique_function_vtable<R(Ts...)>
            >::type;

    public:
        function_base() noexcept
          : vptr(detail::get_empty_function_vtable<vtable>())
          , object(nullptr)
        {}

        function_base(function_base const& other)
          : vptr(other.vptr)
          , object(other.object)
        {
            if (other.object != nullptr)
            {
                object = vptr->copy(
                    storage, detail::function_storage_size, other.object);
            }
        }

        function_base(function_base&& other) noexcept
          : vptr(other.vptr)
          , object(other.object)
        {
            if (object == &other.storage)
            {
                std::memcpy(storage, other.storage, function_storage_size);
                object = &storage;
            }
            other.vptr = detail::get_empty_function_vtable<vtable>();
            other.object = nullptr;
        }

        ~function_base()
        {
            reset();
        }

        function_base& operator=(function_base const& other)
        {
            if (vptr == other.vptr)
            {
                if (this != &other && object)
                {
                    HPX_ASSERT(other.object != nullptr);
                    // reuse object storage
                    vptr->destruct(object);
                    object = vptr->copy(object, -1, other.object);
                }
            } else {
                reset();

                vptr = other.vptr;
                if (other.object != nullptr)
                {
                    object = vptr->copy(
                        storage, detail::function_storage_size, other.object);
                } else {
                    object = nullptr;
                }
            }
            return *this;
        }

        function_base& operator=(function_base&& other) noexcept
        {
            if (this != &other)
            {
                swap(other);
                other.reset();
            }
            return *this;
        }

        void assign(std::nullptr_t) noexcept
        {
            reset();
        }

        template <typename F>
        void assign(F&& f)
        {
            using target_type = typename std::decay<F>::type;
            static_assert(!Copyable ||
                std::is_constructible<target_type, target_type const&>::value,
                "F shall be CopyConstructible");

            if (!is_empty_function(f))
            {
                vtable const* f_vptr = get_vtable<target_type>();
                if (vptr == f_vptr)
                {
                    // reuse object storage
                    vtable::template _destruct<target_type>(object);
                    object = vtable::template construct<target_type>(
                        object, -1, std::forward<F>(f));
                } else {
                    reset();

                    vptr = f_vptr;
                    object = vtable::template construct<target_type>(
                        storage, function_storage_size, std::forward<F>(f));
                }
            } else {
                reset();
            }
        }

        void reset() noexcept
        {
            if (object != nullptr)
            {
                vptr->delete_(object, function_storage_size);

                vptr = detail::get_empty_function_vtable<vtable>();
                object = nullptr;
            }
        }

        void swap(function_base& f) noexcept
        {
            std::swap(vptr, f.vptr);
            std::swap(object, f.object);
            std::swap(storage, f.storage);
            if (object == &f.storage)
                object = &storage;
            if (f.object == &storage)
                f.object = &f.storage;
        }

        bool empty() const noexcept
        {
            return object == nullptr;
        }

        explicit operator bool() const noexcept
        {
            return !empty();
        }

        template <typename T>
        T* target() noexcept
        {
            using target_type = typename std::remove_cv<T>::type;

            static_assert(
                traits::is_invocable_r<R, target_type&, Ts...>::value
              , "T shall be Callable with the function signature");

            vtable const* f_vptr = get_vtable<target_type>();
            if (vptr != f_vptr || empty())
                return nullptr;

            return &vtable::template get<target_type>(object);
        }

        template <typename T>
        T const* target() const noexcept
        {
            using target_type = typename std::remove_cv<T>::type;

            static_assert(
                traits::is_invocable_r<R, target_type&, Ts...>::value
              , "T shall be Callable with the function signature");

            vtable const* f_vptr = get_vtable<target_type>();
            if (vptr != f_vptr || empty())
                return nullptr;

            return &vtable::template get<target_type>(object);
        }

        HPX_FORCEINLINE R operator()(Ts... vs) const
        {
            return vptr->invoke(object, std::forward<Ts>(vs)...);
        }

        std::size_t get_function_address() const
        {
#if defined(HPX_HAVE_THREAD_DESCRIPTION)
            return vptr->get_function_address(object);
#else
            return 0;
#endif
        }

        char const* get_function_annotation() const
        {
#if defined(HPX_HAVE_THREAD_DESCRIPTION)
            return vptr->get_function_annotation(object);
#else
            return nullptr;
#endif
        }

        util::itt::string_handle get_function_annotation_itt() const
        {
#if HPX_HAVE_ITTNOTIFY != 0 && !defined(HPX_HAVE_APEX)
            return vptr->get_function_annotation_itt(object);
#else
            static util::itt::string_handle sh;
            return sh;
#endif
        }

    private:
        template <typename T>
        static vtable const* get_vtable() noexcept
        {
            return detail::get_vtable<vtable, T>();
        }

    protected:
        vtable const *vptr;
        void* object;
        mutable unsigned char storage[function_storage_size];
    };

    template <typename Sig, bool Copyable>
    static bool is_empty_function(function_base<Sig, Copyable> const& f) noexcept
    {
        return f.empty();
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Sig, bool Copyable, bool Serializable>
    class basic_function;

    template <bool Copyable, typename R, typename ...Ts>
    class basic_function<R(Ts...), Copyable, true>
      : public function_base<R(Ts...), Copyable>
    {
        using vtable = typename std::conditional<
                Copyable,
                detail::function_vtable<R(Ts...)>,
                detail::unique_function_vtable<R(Ts...)>
            >::type;
        using serializable_vtable = serializable_function_vtable<vtable>;
        using base_type = function_base<R(Ts...), Copyable>;

    public:
        basic_function() noexcept
          : base_type()
          , serializable_vptr(nullptr)
        {}

        template <typename F>
        void assign(F&& f)
        {
            using target_type = typename std::decay<F>::type;

            base_type::assign(std::forward<F>(f));
            if (!base_type::empty())
            {
                serializable_vptr = get_serializable_vtable<target_type>();
            }
        }

        void swap(basic_function& f) noexcept
        {
            base_type::swap(f);
            std::swap(serializable_vptr, f.serializable_vptr);
        }

    private:
        friend class hpx::serialization::access;

        void save(serialization::output_archive& ar, unsigned const version) const
        {
            bool const is_empty = base_type::empty();
            ar << is_empty;
            if (!is_empty)
            {
                std::string const name = serializable_vptr->name;
                ar << name;

                serializable_vptr->save_object(object, ar, version);
            }
        }

        void load(serialization::input_archive& ar, unsigned const version)
        {
            base_type::reset();

            bool is_empty = false;
            ar >> is_empty;
            if (!is_empty)
            {
                std::string name;
                ar >> name;
                serializable_vptr = detail::get_serializable_vtable<vtable>(name);

                vptr = serializable_vptr->vptr;
                object = serializable_vptr->load_object(
                    storage, function_storage_size, ar, version);
            }
        }

        HPX_SERIALIZATION_SPLIT_MEMBER()

        template <typename T>
        static serializable_vtable const* get_serializable_vtable() noexcept
        {
            return detail::get_serializable_vtable<vtable, T>();
        }

    protected:
        using base_type::vptr;
        using base_type::object;
        using base_type::storage;
        serializable_vtable const* serializable_vptr;
    };

    template <bool Copyable, typename R, typename ...Ts>
    class basic_function<R(Ts...), Copyable, false>
      : public function_base<R(Ts...), Copyable>
    {};

    template <typename Sig, bool Copyable, bool Serializable>
    static bool is_empty_function(
        basic_function<Sig, Copyable, Serializable> const& f) noexcept
    {
        return f.empty();
    }
}}}

#endif
