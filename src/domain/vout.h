#ifndef BTCR_DID_VOUT_H
#define BTCR_DID_VOUT_H


/**
 * This class represents a VOUT, an index identifying a particular output in a transaction
 */
class Vout {
public:
    /**
     * Construct a Vout from the given index
     * @param index the index of the particular output in a transaction
     */
    explicit Vout(int index);

    /**
     * Get this Vout as an integer value
     * @return this Vout as an integer value
     */
    int value();

    bool operator==(const Vout &rhs) const;

    bool operator!=(const Vout &rhs) const;

private:
    int index;
};


#endif //BTCR_DID_VOUT_H
