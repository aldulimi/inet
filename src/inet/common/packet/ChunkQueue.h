//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#ifndef __INET_CHUNKQUEUE_H_
#define __INET_CHUNKQUEUE_H_

#include "inet/common/packet/chunk/BitsChunk.h"
#include "inet/common/packet/chunk/BytesChunk.h"
#include "inet/common/packet/chunk/EmptyChunk.h"

namespace inet {

/**
 * This class represents a first in, first out queue of chunks. It is mainly
 * useful for application and protocol buffers where the incoming chunks are
 * guaranteed to be in order.
 *
 * Internally, queues stores the data in different kind of chunks. See the
 * Chunk class and its subclasses for details. All chunks are immutable in a
 * queue. Chunks are automatically merged as they are pushed into the queue,
 * and they are also shared among queues when duplicating.
 *
 * In general, this class supports the following operations:
 *  - push at the tail and pop at the head
 *  - query the length
 *  - serialize to and deserialize from a sequence of bits or bytes
 *  - copy to a new queue
 *  - convert to a human readable string
 */
class INET_API ChunkQueue : public cNamedObject
{
  friend class ChunkQueueDescriptor;

  protected:
    bit pushedLength = bit(0);
    bit poppedLength = bit(0);
    /**
     * This chunk is always immutable to allow arbitrary peeking. Nevertheless
     * it's reused if possible to allow efficient merging with newly added chunks.
     */
    std::shared_ptr<Chunk> contents;
    Chunk::Iterator iterator;

  protected:
    Chunk *getContents() const { return contents.get(); } // only for class descriptor

    void remove(bit length);

  public:
    /** @name Constructors, destructors and duplication related functions */
    //@{
    ChunkQueue(const char *name = nullptr, const std::shared_ptr<Chunk>& contents = EmptyChunk::singleton);
    ChunkQueue(const ChunkQueue& other);

    virtual ChunkQueue *dup() const override { return new ChunkQueue(*this); }
    //@}

    /** @name Length querying related functions */
    //@{
    /**
     * Returns the total length of data currently available in the queue.
     */
    bit getLength() const { return contents->getChunkLength() - iterator.getPosition(); }

    /**
     * Returns the total length of data pushed into the queue so far.
     */
    bit getPushedLength() const { return pushedLength; }

    /**
     * Returns the total length of data popped from the queue so far.
     */
    bit getPoppedLength() const { return poppedLength; }
    //@}

    /** @name Querying data related functions */
    //@{
    /**
     * Returns the designated data from the head of the queue as an immutable
     * chunk in its current representation. If the length is unspecified, then
     * the length of the result is chosen according to the internal representation.
     */
    std::shared_ptr<Chunk> peek(bit length = bit(-1), int flags = 0) const;

    /**
     * Returns the designated data at the given offset as an immutable chunk in
     * its current representation. If the length is unspecified, then the length
     * of the result is chosen according to the internal representation.
     */
    std::shared_ptr<Chunk> peekAt(bit offset, bit length, int flags = 0) const;

    /**
     * Returns true if the designated data is available at the head of the queue
     * in the requested representation. If the length is unspecified, then the
     * length of the result is chosen according to the internal representation.
     */
    template <typename T>
    bool has(bit length = bit(-1)) const {
        return contents->has<T>(iterator, length);
    }

    /**
     * Returns the designated data from the head of the queue as an immutable
     * chunk in the requested representation. If the length is unspecified, then
     * the length of the result is chosen according to the internal representation.
     */
    template <typename T>
    std::shared_ptr<T> peek(bit length = bit(-1), int flags = 0) const {
        return contents->peek<T>(iterator, length, flags);
    }

    /**
     * Returns the designated data at the given offset as an immutable chunk in
     * the requested representation. If the length is unspecified, then the
     * length of the result is chosen according to the internal representation.
     */
    template <typename T>
    std::shared_ptr<T> peekAt(bit offset, bit length = bit(-1), int flags = 0) const {
        return contents->peek<T>(Chunk::Iterator(true, iterator.getPosition() + offset, -1), length, flags);
    }

    /**
     * Returns all data in the queue in the current representation. The length
     * of the returned chunk is the same as the value returned by getLength().
     */
    std::shared_ptr<Chunk> peekAll(int flags = 0) const {
        return peekAt(bit(0), getLength(), flags);
    }

    /**
     * Returns all data in the queue in the as a sequence of bits. The length
     * of the returned chunk is the same as the value returned by getLength().
     */
    std::shared_ptr<BitsChunk> peekAllBits(int flags = 0) const {
        return peekAt<BitsChunk>(bit(0), getLength(), flags);
    }

    /**
     * Returns all data in the queue in the as a sequence of bytes. The length
     * of the returned chunk is the same as the value returned by getLength().
     */
    std::shared_ptr<BytesChunk> peekAllBytes(int flags = 0) const {
        return peekAt<BytesChunk>(bit(0), getLength(), flags);
    }
    //@}

    /** @name Removing data related functions */
    //@{
    /**
     * Pops the designated data and returns it as an immutable chunk in its
     * current representation. If the length is unspecified, then the length of
     * the result is chosen according to the internal representation.
     */
    std::shared_ptr<Chunk> pop(bit length = bit(-1), int flags = 0);

    /**
     * Pops the designated data from the head of the queue and returns it as
     * an immutable chunk in the requested representation. If the length is
     * unspecified, then the length of the result is chosen according to the
     * internal representation.
     */
    template <typename T>
    std::shared_ptr<T> pop(bit length = bit(-1), int flags = 0) {
        const auto& chunk = peek<T>(length, flags);
        if (chunk != nullptr)
            remove(chunk->getChunkLength());
        return chunk;
    }

    /**
     * Erases all data from the queue.
     */
    void clear();
    //@}

    /** @name Filling with data related functions */
    //@{
    /**
     * Inserts the provided chunk at the tail of the queue.
     */
    void push(const std::shared_ptr<Chunk>& chunk);
    //@}

    /**
     * Returns a human readable string representation.
     */
    virtual std::string str() const override { return contents->str(); }
};

inline std::ostream& operator<<(std::ostream& os, const ChunkQueue *queue) { return os << queue->str(); }

inline std::ostream& operator<<(std::ostream& os, const ChunkQueue& queue) { return os << queue.str(); }

} // namespace

#endif // #ifndef __INET_CHUNKQUEUE_H_

