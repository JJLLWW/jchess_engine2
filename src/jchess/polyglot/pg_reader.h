#pragma once

#include "../board.h"

#include <mio/mmap.hpp>
#include <optional>

namespace jchess::polyglot {
    class PGZobristHasher final : public ZobristHasher {
    public:
        PGZobristHasher();
    private:
        int get_piece_offset(Square square, Piece piece) const override;
        int get_castle_offset(CastleBits castle_type) const override;
        int get_enp_offset(Board const& board) const override;
        bool should_use_turn_value(Color color) const override;
    };

    Move read_pg_move(uint16_t move, Board const& board);

    constexpr int BOOK_ENTRY_BYTES = 16;

    struct PGBookEntry {
        uint64_t key;
        uint16_t move;
        uint16_t weight;
        uint32_t learn;
    };

    constexpr bool operator<(PGBookEntry const& lhs, PGBookEntry const& rhs) { return lhs.key < rhs.key; }

    class PGMappedBook;

    class PGBookIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = PGBookEntry;
        using pointer = const PGBookEntry*;
        using reference = PGBookEntry&;
        using difference_type = size_t;
    public:
        PGBookIterator(PGMappedBook const* book, size_t index) : book{book}, cur_index{index} {}
        PGBookIterator& operator=(PGBookIterator const& other);
        PGBookIterator& operator++();
        PGBookEntry operator*();
        friend bool operator==(PGBookIterator const& lhs, PGBookIterator const& rhs);
    private:
        PGMappedBook const *book;
    public:
        size_t cur_index;
    };

    class PGMappedBook {
    public:
        PGMappedBook() = default;
        PGMappedBook(std::string const& pg_book_path);
        void map_file(std::string const& pg_book_path);
        size_t size() const; // number of entries
        PGBookEntry operator[](size_t index) const; // entry index
        PGBookIterator begin() const;
        PGBookIterator end() const;
        std::optional<Move> get_random_book_move(Board const& board) const;
    private:
        PGZobristHasher hasher;
        mio::ummap_source mapping;
    };
}