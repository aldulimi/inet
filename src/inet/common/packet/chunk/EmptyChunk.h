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

#ifndef __INET_EMPTYCHUNK_H_
#define __INET_EMPTYCHUNK_H_

#include "inet/common/packet/chunk/Chunk.h"

namespace inet {

/**
 * This class represents a completely empty chunk.
 */
class INET_API EmptyChunk : public Chunk
{
  friend class Chunk;

  public:
    static std::shared_ptr<EmptyChunk> singleton;

  protected:
    virtual std::shared_ptr<Chunk> peekUnchecked(PeekPredicate predicate, PeekConverter converter, const Iterator& iterator, bit length, int flags) const override;

    static std::shared_ptr<Chunk> convertChunk(const std::type_info& typeInfo, const std::shared_ptr<Chunk>& chunk, bit offset, bit length, int flags);

  public:
    /** @name Constructors, destructors and duplication related functions */
    //@{
    EmptyChunk();
    EmptyChunk(const EmptyChunk& other);

    virtual EmptyChunk *dup() const override { return new EmptyChunk(*this); }
    virtual std::shared_ptr<Chunk> dupShared() const override { return std::make_shared<EmptyChunk>(*this); }
    //@}

    /** @name Overridden chunk functions */
    //@{
    virtual ChunkType getChunkType() const override { return CT_EMPTY; }
    virtual bit getChunkLength() const override { return bit(0); }

    virtual std::string str() const override;
    //@}
};

} // namespace

#endif // #ifndef __INET_EMPTYCHUNK_H_

