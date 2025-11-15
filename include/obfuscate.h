#ifndef OBFUSCATE_H
#define OBFUSCATE_H

#include <string>
#include <array>
#include <cstddef>

namespace Obfuscation {
    template <size_t N>
    class XorStr {
    private:
        static constexpr char key_ = 0x42; // A simple XOR key
        std::array<char, N> data_;

    public:
        constexpr XorStr(const char* str) : data_{} {
            for (size_t i = 0; i < N; ++i) {
                data_[i] = str[i] ^ key_;
            }
        }

        std::string get() const {
            std::string decrypted;
            decrypted.resize(N - 1);
            for (size_t i = 0; i < N - 1; ++i) {
                decrypted[i] = data_[i] ^ key_;
            }
            return decrypted;
        }
    };
}

#define OBFUSCATE(s) (Obfuscation::XorStr<sizeof(s)>(s).get())

#endif // OBFUSCATE_H
