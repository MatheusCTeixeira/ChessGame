#pragma once

#include "game_types.hpp"

#include <algorithm>
#include <cstdint>
#include <exception>
#include <initializer_list>
#include <list>
#include <string>
#include <vector>

#ifndef NDEBUB
#include <iostream>

using namespace std;

#endif

namespace ch {

class history_entry_t {
    friend class game_t;

public:

    // ordinary move.
    static history_entry_t make_simple_move(
        pos_t from, pos_t to, piece_t piece);

    // ordinary attack move.
    static history_entry_t make_attack_move(
        pos_t from, pos_t to, piece_t piece_before, piece_t piece_after);

    // en-passant move.
    // the color, position are implicit.
    static history_entry_t make_en_passant_move(pos_t from, pos_t to);

    // castling move.
    // the envolved pieces are implicit.
    static history_entry_t make_castling_move(pos_t from, pos_t to);

    // promotion.
    // piece before, piece after: selected promotion piece.
    static history_entry_t make_promotion_move(
        pos_t from, pos_t to, piece_t piece_before, piece_t piece_after);


    history_entry_t() = default;
    history_entry_t(const history_entry_t&) = default;

    ~history_entry_t();

private:
    enum class type_t: uint8_t {
        simple, attack, en_passant, castling, promotion };

    explicit history_entry_t(
        type_t type, pos_t from, pos_t to, piece_t state_before = piece_t::empt,
        piece_t state_after = piece_t::empt);

    type_t _type;
    piece_t _state[2];
    pos_t _from, _to;
};

class game_t {
public:
#ifndef NDEBUG
    std::string piece_to_str(piece_t) const;
    void print_board() const;
#endif

    using list_pos_t = std::list<pos_t>;

    explicit game_t();
    ~game_t();

    std::size_t hash() const;
    bool test_check_mate(color_t color) const;
    bool test_check(color_t color) const;
    void reset();

    void undo();
    void redo();

    // must return a ch::piece_t an take 0 (any) parameter.
    template <typename Callable>
    void promotion_handler(Callable callabe);

    bool move(pos_t from, pos_t to, bool commit = true);
    bool can_move(pos_t from, pos_t to) const;

    list_pos_t list_moves(pos_t pos) const;

    // list all pieces that can move to specified position.
    list_pos_t can_move_to(pos_t to, color_t color) const;
    list_pos_t can_move_to(pos_t to, piece_t color) const;

protected:
    void print_iterators() const {
        for (auto pos: _w_pieces)
            cout << "[" << (int)pos._i << ", " << (int)pos._j << "], ";
        cout << "\n";
        for (auto pos: _b_pieces)
            cout << "[" << (int)pos._i << ", " << (int)pos._j << "], ";
    }

    void set_piece(pos_t pos, piece_t piece);
    piece_t get_piece(pos_t pos) const;

    list_pos_t list_king_moves(pos_t origin) const;
    list_pos_t list_queen_moves(pos_t origin) const;
    list_pos_t list_bishop_moves(pos_t origin) const;
    list_pos_t list_knight_moves(pos_t origin) const;
    list_pos_t list_rook_moves(pos_t origin) const;
    list_pos_t list_pawn_moves(pos_t origin) const;
    list_pos_t list_moves(
        pos_t origin, std::size_t range, bool parallel, bool diagonal) const;

    void undo_simple_move(history_entry_t past_move);
    void undo_attack_move(history_entry_t past_move);
    void undo_castling_move(history_entry_t past_move);
    void undo_promotion_move(history_entry_t past_move);
    void undo_en_passant_move(history_entry_t past_move);

    void redo_simple_move(history_entry_t new_move);
    void redo_attack_move(history_entry_t new_move);
    void redo_castling_move(history_entry_t new_move);
    void redo_promotion_move(history_entry_t new_move);
    void redo_en_passant_move(history_entry_t new_move);

    bool move_pawn(pos_t from, pos_t to, bool commit = true);
    bool move_king(pos_t from, pos_t to, bool commit = true);
    bool move_queen(pos_t from, pos_t to, bool commit = true);
    bool move_knight(pos_t from, pos_t to, bool commit = true);
    bool move_bishop(pos_t from, pos_t to, bool commit = true);
    bool move_rook(pos_t from, pos_t to, bool commit = true);

    void commit_move(pos_t from, pos_t to);

    bool empty_diagonal(pos_t from, pos_t to) const;
    bool empty_parallel(pos_t from, pos_t to) const;

    bool can_en_passant(pos_t from, pos_t to) const;
    void do_en_passant(pos_t from, pos_t to);

    bool can_castling(pos_t from, pos_t to) const;
    void do_castling(pos_t from, pos_t to);

    void push_history(history_entry_t history_entry);
    history_entry_t pop_history();
    history_entry_t top_history() const;

    bool same_col(pos_t p1, pos_t p2) const;
    inline bool same_j(pos_t p1, pos_t p2) const;
    inline bool same_i(pos_t p1, pos_t p2) const;
    inline bool is_empty(pos_t p) const;

    void add_to_iterator(color_t col, pos_t pos);
    void update_iterator(pos_t old_pos, pos_t new_pos);
    void remove_from_iterator(pos_t pos);
    void reset_iterators();

    pos_t find_king(color_t color) const;
    bool is_castling(pos_t from, pos_t to) const;

private:
    std::vector<history_entry_t> _history;
    uint16_t _history_head;
    piece_t _board[8][8];

    // just iterators to enhance algorithms performance.
    std::list<pos_t> _b_pieces;
    std::list<pos_t> _w_pieces;
};

}

namespace std {

template <>
class hash<ch::game_t>
{
public:
    size_t operator()(const ch::game_t& game) const
    {
        size_t result = 0x12345678;
        result |= (~game.hash() << 32);
        return result;
    }
};

}