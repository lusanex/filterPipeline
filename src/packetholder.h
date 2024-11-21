#ifndef PACKET_HOLDER_H
#define PACKET_HOLDER_H

#include "typeid.h"
#include "packetexception.h"

template <typename T>
class PacketHolder {
private:
    T* data;                // Raw pointer to the data.
    TypeId typeId;          // TypeId of the stored type.

public:
    // Constructor for lvalue data.
    explicit PacketHolder(const T& value)
        : data(new T(value)), typeId(TypeId::Of<T>()) {}

    // Constructor for rvalue data.
    explicit PacketHolder(T&& value)
        : data(new T(std::move(value))), typeId(TypeId::Of<T>()) {}

    // Destructor.
    ~PacketHolder() { delete data; }

    // Type validation.
    template <typename U>
    void validateType() const {
        TypeId otherTypeId = TypeId::Of<U>();
        if (typeId != TypeId::Of<U>()) {
            throw PacketException("Invalid packet types. Expected: " +
                                    typeId.getId() + "got: " + otherTypeId.getId());
        }
    }

    // Access data with type validation.
    template <typename U>
    const U& get() const {
        validateType<U>();
        return *data;
    }

    // Allow Packet access to internals.
    friend class Packet;
};


#endif
