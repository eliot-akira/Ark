#ifndef ark_vm_value
#define ark_vm_value

#include <vector>
#include <variant>
#include <string>
#include <cinttypes>
#include <iostream>
#include <memory>
#include <functional>
#include <utility>

#include <Ark/VM/Types.hpp>
#include <Ark/VM/Closure.hpp>
#include <Ark/Exceptions.hpp>
#include <Ark/VM/UserType.hpp>

#define ARK_MAX(a, b) ((a) > (b) ? (a) : (b))
// getting the max size in bytes to store all of our data
#define ARK_MAX_SIZE ARK_MAX(\
    sizeof(double), \
    ARK_MAX(\
        sizeof(std::string), \
        ARK_MAX(\
            sizeof(PageAddr_t), \
            ARK_MAX(\
                sizeof(NFT), \
                ARK_MAX(\
                    sizeof(ProcType), \
                    ARK_MAX(\
                        sizeof(Closure), \
                        ARK_MAX(\
                            sizeof(UserType), \
                            sizeof(std::vector<Value>))))))))

namespace Ark
{
    template<bool D> class VM_t;
}

namespace Ark::internal
{
    enum class ValueType
    {
        List,
        Number,
        String,
        PageAddr,
        NFT,
        CProc,
        Closure,
        User
    };

    class Frame;

    class Value
    {
    public:
        using ProcType  = std::function<Value (std::vector<Value>&)>;
        using Iterator  = std::vector<Value>::const_iterator;

        Value();
        Value(const Value& other);
        ~Value();

        Value& operator=(const Value& other);

        Value(ValueType type);
        Value(int value);
        Value(float value);
        Value(double value);
        Value(const std::string& value);
        Value(std::string&& value);
        Value(PageAddr_t value);
        Value(NFT value);
        Value(Value::ProcType value);
        Value(std::vector<Value>&& value);
        Value(Closure&& value);
        Value(UserType&& value);

        inline ValueType valueType() const;
        inline bool isFunction() const;
        inline double number() const;
        inline const std::string& string() const;
        inline const std::vector<Value>& const_list() const;
        inline const UserType& usertype() const;

        std::vector<Value>& list();
        std::string& string_ref();
        UserType& usertype_ref();

        // utilities working only with lists
        void push_back(const Value& value);
        void push_back(Value&& value);

        template <typename... Args>
        Value resolve(Args&&... args) const;

        friend std::ostream& operator<<(std::ostream& os, const Value& V);
        friend inline bool operator==(const Value& A, const Value& B);
        friend inline bool operator<(const Value& A, const Value& B);
        friend inline bool operator!(const Value& A);

        template<bool D> friend class Ark::VM_t;

    private:
        uint8_t m_data[ARK_MAX_SIZE];
        void* m_value;
        ValueType m_type;
        bool m_const;

        inline void copyValue(const Value& other);

        Ark::VM_t<false>* m_vmf = nullptr;
        Ark::VM_t<true>* m_vmt = nullptr;

        inline PageAddr_t pageAddr() const;
        inline NFT nft() const;
        inline const ProcType& proc() const;
        inline const Closure& closure() const;
        Closure& closure_ref();

        // the vm is registered as a pointer when a call instruction is met
        // because plugins needs to access the vm to run some function calls
        void registerVM(Ark::VM_t<false>* vm);
        void registerVM(Ark::VM_t<true>* vm);
    };
    
    #include "Value.inl"
}

#undef ARK_MAX
#undef ARK_MAX_SIZE

#endif