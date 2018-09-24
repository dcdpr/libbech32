#ifndef BTCR_DID_TRANSACTIONPOSITION_H
#define BTCR_DID_TRANSACTIONPOSITION_H

/**
 * This class represents a TransactionPosition, an index identifying a particular transaction in a block
 */
class TransactionPosition {
public:
    /**
     * Construct a TransactionPosition from the given index
     * @param inPosition the index of the particular transaction in a block
     */
    explicit TransactionPosition(int inPosition);

    /**
     * Get this TransactionPosition as an integer value
     * @return this TransactionPosition as an integer value
     */
    int value();

    bool operator==(const TransactionPosition &rhs) const;

    bool operator!=(const TransactionPosition &rhs) const;

private:
    int position;
};


#endif //BTCR_DID_TRANSACTIONPOSITION_H
