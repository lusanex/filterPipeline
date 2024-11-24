#ifndef PACKET_HOLDER_H
#define PACKET_HOLDER_H

#include <memory>
#include "typeid.h"  // Assuming you have a custom TypeId implementation
#include "packetexception.h"  // Assuming this is a custom exception class

class PacketHolderBase {
public:
    virtual ~PacketHolderBase() = default;
};

template <typename T>
class PacketHolder : public PacketHolderBase {
private:
    std::unique_ptr<T> data;  // Use smart pointer for memory safety
    TypeId typeId;            // TypeId of the stored type

public:
    // Constructor for lvalue data
    explicit PacketHolder(const T& value)
        : data(std::make_unique<T>(value)), typeId(TypeId::Of<T>()) {}

    // Constructor for rvalue data
    explicit PacketHolder(T&& value)
        : data(std::make_unique<T>(std::move(value))), typeId(TypeId::Of<T>()) {}

    // Copy constructor
    PacketHolder(const PacketHolder& other)
        : data(std::make_unique<T>(*other.data)), typeId(other.typeId) {}

    // Copy assignment operator
    PacketHolder& operator=(const PacketHolder& other) {
        if (this != &other) {
            data = std::make_unique<T>(*other.data);  // Deep copy
            typeId = other.typeId;
        }
        return *this;
    }

    // Move constructor
    PacketHolder(PacketHolder&& other) noexcept
        : data(std::move(other.data)), typeId(other.typeId) {}

    // Move assignment operator
    PacketHolder& operator=(PacketHolder&& other) noexcept {
        if (this != &other) {
            data = std::move(other.data);  // Transfer ownership
            typeId = other.typeId;
        }
        return *this;
    }

    // Destructor
    ~PacketHolder() override = default;  // Smart pointers handle cleanup

    // Type validation
    template <typename U>
    void validateType() const {
        if (typeId != TypeId::Of<U>()) {
            throw PacketException("Invalid packet types. Expected: " +
                                  TypeId::Of<U>().getId() + ", got: " +
                                  typeId.getId());
        }
    }
    template <typename U>
    const T& get() const{
        validateType<U>();
        return *data;

    }

    // Access data with type validation
    const T& get() const{
        return *data;
    }

    T& get() {
        return *data;
    }


};

#endif  // PACKET_HOLDER_H

