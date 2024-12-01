/**********************************
 * @file packetholder.h
 * @author Erich Gutierrez Chavez
 * @brief Defines the PacketHolder class template and 
 *  base class for managing packet data.
 *
 * @details
 * - PacketHolderBase and PacketHolder template classes.
 * - PacketHolderBase provides a polymorphic base class for packet management.
 * - PacketHolder is a templated class that manages packet data with deep copy, 
 *   move semantics, and type safety.
 *
 * Constraints:
 * - The template type `T` must be copyable and movable, 
 *   as deep copies and moves are implemented.
 * - Ensures encapsulation by managing data through `unique_ptr`.
 **********************************/

#ifndef PACKET_HOLDER_H
#define PACKET_HOLDER_H

#include <memory>
#include "packetexception.h"  
using namespace std;

/**********************************
 * Base class for packet holders, 
 * enabling polymorphism
 **********************************/
class PacketHolderBase {
public:
    /**********************************
     * Virtual destructor to ensure proper 
     * cleanup for derived classes.
     **********************************/
    virtual ~PacketHolderBase() = default;
};

/**********************************
 * @class PacketHolder
 * @brief A templated class for managing packet data 
 * with deep copy and move semantics.
 * @tparam T The type of data to be managed.
 **********************************/
template <typename T>
class PacketHolder : public PacketHolderBase {
    private:
        unique_ptr<T> data;  

    public:
        /**********************************
         * Constructs a PacketHolder with a 
         * copy of the provided data.
         * @param value The data to be copied 
         *        into the PacketHolder.
         **********************************/
        explicit PacketHolder(const T& value)
            : data(make_unique<T>(value)) {}

        /**********************************
         * Constructs a PacketHolder by moving the provided data.
         * @param value The data to be moved into the PacketHolder.
         **********************************/
        explicit PacketHolder(T&& value)
            : data(make_unique<T>(std::move(value))) {}

        /**********************************
         * Copy constructor for deep copying another PacketHolder.
         * @param other The PacketHolder to be copied.
         **********************************/
        PacketHolder(const PacketHolder& other)
            : data(make_unique<T>(*other.data)) {}

        /**********************************
         * Copy assignment operator for deep copying another PacketHolder.
         * @param other The PacketHolder to be copied.
         * @return A reference to the current object after the copy.
         **********************************/
        PacketHolder& operator=(const PacketHolder& other) {
            if (this != &other) {
                data = make_unique<T>(*other.data);  // Deep copy
            }
            return *this;
        }

        /**********************************
         * Move constructor to transfer ownership of data from another PacketHolder.
         * @param other The PacketHolder to be moved from.
         **********************************/
        PacketHolder(PacketHolder&& other) noexcept
            : data(std::move(other.data)) {}

        /**********************************
         * Move assignment operator to transfer ownership of data from another PacketHolder.
         * @param other The PacketHolder to be moved from.
         * @return A reference to the current object after the move.
         **********************************/
        PacketHolder& operator=(PacketHolder&& other) noexcept {
            if (this != &other) {
                data = std::move(other.data);  // Transfer ownership
            }
            return *this;
        }

        /**********************************
         * Destructor to clean up the managed data.
         **********************************/
        ~PacketHolder() override = default;  

        /**********************************
         * Retrieves the data as a constant reference.
         * @return A constant reference to the managed data.
         **********************************/
        const T& get() const {
            return *data;
        }

        /**********************************
         * Retrieves the data as a mutable reference.
         * @return A mutable reference to the managed data.
         **********************************/
        T& get() {
            return *data;
        }
};

#endif  // PACKET_HOLDER_H

