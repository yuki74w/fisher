#pragma once

#include<iostream>
#include<functional>
#include<vector>
#include<array>
#include<iterator>

namespace binary {
	using byte = int8_t;
	using word = int16_t;
	using dword = int32_t;
	using qword = int64_t;

	namespace endian {
		enum class type {
			null,
			little,
			big,
			both
		};
	}

	class bytes : public std::vector<byte> {
	private:
	public:
		enum class option_type {
			none,
			not_eof
		};

		bytes() = default;
		bytes(const bytes&) = default;
		bytes(bytes&&) = default;

		endian::type mode = endian::type::null;
		option_type option = option_type::none;

		bytes(byte b) {
			push_back(b);
		}


		bytes& endianed(endian::type mode) {
			if (mode == endian::type::null) {
				throw "not dicided mode";
			}
			if (mode == endian::type::little) {
				std::reverse(begin(), end());
			}
			if (mode == endian::type::big) {
			}
			if (mode == endian::type::both) {
				bytes ret = *this;
				std::reverse(ret.begin(), ret.end());
				*this << ret;
			}
			return *this;
		}

		bytes& optioned(option_type option) {
			if (option == option_type::not_eof) {
				pop_back();
			}
			return *this;
		}

		template<typename T>
		bytes& operator<<(T&& a);

		bytes& operator<<(bytes&& a) {

			if (size() == 0)
				push_back(a[0]);
			else
				insert(end(), a.begin(), a.end()); //連結するstd::vector<byte>のサイズはよくわからないのでstd::copyは使わないでおく

			option = option_type::none;
			return *this;
		}



		bytes& operator<<(endian::type  mode) {
			this->mode = mode;
			return *this;
		}

		bytes& operator<<(option_type  mode) {
			this->option = option;
			return *this;
		}

	};

	std::ostream& operator<<(std::ostream& os, const bytes &b) {
		for (auto&& elem : b) {
			os << elem;
		}
		return os;
	}

	namespace detail {
		template<typename T>
		struct is_array : public std::false_type {
			static constexpr size_t size = 0;
		};

		template<typename T, size_t N>
		struct is_array <T[N]> : public std::true_type {
			using value_type = T;
			static constexpr size_t size = N;
		};

		template <class T>
		class has_iterator {
			template <class U>
			static constexpr bool check(typename U::iterator*)
			{
				return true;
			}

			template <class U>
			static constexpr bool check(...)
			{
				return false;
			}
		public:
			static constexpr bool value = check<T>(nullptr);
		};


		template<typename T,typename = void>
		struct helper {
			static bytes cast(const T& val) {

				return bytes(byte(val));
			}
		};

		template<typename T>
		struct helper<T, typename std::enable_if<has_iterator<T>::value>::type> {
			static bytes cast(const T& val) {
				bytes ret;
				for (auto &&elem : val) {
					ret << cast(val);
				}
				return ret;
			}
		};

		template<typename T>
		struct helper<T,typename is_array<T>::value_type> {
			static bytes cast(const T& val) {
				bytes ret;
				for (auto &&elem : val) {
					ret << cast(val);
				}
			}
		};

		template<class T>
		bytes cast(const T& val) {
			return helper<T>::cast(val);
		}

		bytes cast(byte val) {
			return bytes(val);
		}

		bytes cast(const bytes& val) {
			return val;
		}

		bytes cast(word val) {
			return (cast((val & 0xFF00) >> 8) << endian::type::big <<  cast(val & 0x00FF));
		}

		bytes cast(dword val) {
			return (cast(word((val & 0xFFFF0000) >> 16)) << endian::type::big <<  cast(word(val & 0xFFFF)));
		}

		bytes cast(qword val) {
			return (cast(dword((val & 0xFFFFFFFF00000000) >> 32)) << endian::type::big << cast(dword(val & 0xFFFFFFFF)));
		}

		bytes cast(const char* val) {
			char elem = *val;
			bytes ret;
			while (elem != '\0') {
				ret << cast(elem);
			}
			return ret;
		}
	}

	template<typename T>
	bytes& bytes::operator<<(T&& a) {
		auto b = detail::cast(a).endianed(mode).optioned(option);

		insert(end(), b.begin(), b.end()); //連結するstd::vector<byte>のサイズはよくわからないのでstd::copyは使わないでおく

		option = option_type::none;
		return *this;
	}
}