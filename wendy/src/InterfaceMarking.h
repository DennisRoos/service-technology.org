#ifndef _INTERFACEMARKING_H
#define _INTERFACEMARKING_H

#include <ostream>
#include "types.h"


/*! 
  \brief an interface marking
  
  Interface markings are stored as a mapping from an input/output label to
  integer numbers that range from 0 to the message bound. To save space,
  several numbers are stored within a byte when possible.
  
  The set(), inc(), and dec() operator check whether the stored values stay
  in their bounds and return false if they detect a problem (i.e. violation
  of the message bound or decrement of 0). Once such a problem was detected,
  the whole marking might be invalid and should not be used any more.
  
  \note The positions of the interface markings are the same as for the
        respective labels. That is, they range from Label::first_receive to
        Label::last_send. Internally, they are stored in the range from 0
        to Label::last_send-1. The operators take care of this, so get(1)
        returns the marking of label 1 which is stored at position 0.
    
  \note As the values are stored as uint8_t, they are treated like unsigned
        char. Hence, you need to cast the result of get() to unsigned int in
        order to display the marking count in a stream operator. The
        operator<< does this voluntarily.
  
  \todo see http://c-faq.com/misc/bitsets.html for possible improvements --
        especially using preprocessor macros
*/
class InterfaceMarking {
    public: /* static functions */
    
        /// initializes the class InterfaceMarking
        static unsigned int initialize(unsigned int);
        
    private: /* static attributes */
    
        /// the message bound
        static unsigned int message_bound;
        
        /// the size of the asynchronous interface (send + receive)
        static unsigned int interface_length;
        
        /// how many bits are needed to store a single message bound
        static unsigned int message_bound_bits;
        
        /// how many bytes are needed to store an interface marking
        static unsigned int bytes;
        
        /// how many markings can be stored in one byte
        static unsigned int markings_per_byte;
            
    public: /* member functions */
    
        /// constructor
        InterfaceMarking();
        
        /// copy constructor
        InterfaceMarking(const InterfaceMarking &);

        /// copy constructor with effect on given label
        InterfaceMarking(const InterfaceMarking &, Label_ID, bool, bool&);
        
        /// destructor
        ~InterfaceMarking();


        /// comparison operator
        bool operator< (const InterfaceMarking &other) const;

        /// comparison operator
        bool operator== (const InterfaceMarking &other) const;

        /// comparison operator
        bool operator!= (const InterfaceMarking &other) const;

        /// stream output operator
        friend std::ostream& operator<< (std::ostream&, const InterfaceMarking&);


        /// returns the marking value for the given label
        uint8_t get(Label_ID) const;

        /// sets the marking value for the given label to the given value
        bool set(Label_ID, uint8_t&);

        /// increments the value at the given label
        bool inc(Label_ID);

        /// decrements the value at the given label
        bool dec(Label_ID);

        /// returns whether this marking is empty
        bool empty() const;

        /// returns the hash value of this marking
        hash_t hash() const;

    private: /* member attributes */

        /// a byte array to store the interface markings
        uint8_t *storage;
};

#endif
