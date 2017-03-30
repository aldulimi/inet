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

#include "inet/common/packet/chunk/EmptyChunk.h"

namespace inet {

std::shared_ptr<EmptyChunk> EmptyChunk::singleton = std::make_shared<EmptyChunk>();

EmptyChunk::EmptyChunk() :
    Chunk()
{
    markImmutable();
}

EmptyChunk::EmptyChunk(const EmptyChunk& other) :
    Chunk(other)
{
    markImmutable();
}

std::shared_ptr<Chunk> EmptyChunk::peekUnchecked(PeekPredicate predicate, PeekConverter converter, const Iterator& iterator, bit length, int flags) const
{
    assert(iterator.getPosition() == bit(0));
    assert(length == bit(0) || length == bit(-1));
    // 1. peeking returns nullptr
    if (predicate == nullptr || predicate(nullptr))
        return nullptr;
    // 2. peeking returns this chunk
    auto result = const_cast<EmptyChunk *>(this)->shared_from_this();
    if (predicate == nullptr || predicate(result))
        return result;
    // 3. peeking with conversion
    return converter(const_cast<EmptyChunk *>(this)->shared_from_this(), iterator, length, flags);
}

std::shared_ptr<Chunk> EmptyChunk::convertChunk(const std::type_info& typeInfo, const std::shared_ptr<Chunk>& chunk, bit offset, bit length, int flags)
{
    assert(length == bit(0));
    return std::make_shared<EmptyChunk>();
}

std::string EmptyChunk::str() const
{
    return "EmptyChunk";
}

} // namespace
