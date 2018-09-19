
#ifndef BTCR_DID_BLOCKHEIGHT_H
#define BTCR_DID_BLOCKHEIGHT_H

/**
 * This class represents a BlockHeight, an index identifying a particular block in the chain
 */
class BlockHeight {
public:
    /**
     * Construct a BlockHeight from the given index
     * @param inHeight the index of the particular block in the chain
     */
    explicit BlockHeight(int inHeight);

    /**
     * Get this BlockHeight as an integer value
     * @return this BlockHeight as an integer value
     */
    int value();

    bool operator==(const BlockHeight &rhs) const;

    bool operator!=(const BlockHeight &rhs) const;

private:
    int height;
};


#endif //BTCR_DID_BLOCKHEIGHT_H
