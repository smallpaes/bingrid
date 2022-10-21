#include "bingrid.h"

typedef enum
{
  row,
  column
} target;

bool apply_pairs(board *brd, int i, int j);
bool apply_pairs2_target(board *brd, int i, int j, target t);
bool apply_oxo(board *brd, int i, int j);
bool apply_oxo2_target(board *brd, int i, int j, target t);
bool apply_counting(board *brd);
void increment_val(char v, int *z_cnt, int *o_cnt);
int get_str_lng(char s[]);
char get_opposite_val(char c);
bool validate_grid(board *brd);
bool check_grid_finished(board *brd);
bool apply_val2_target(board *brd, int i, int zero_cnt, int one_cnt, target t);
bool check_is_int(float f);
void printboard(board *brd);
bool validate_char(char c);
bool cell_has_val(char c);

bool str2board(board *brd, char *str)
{
  bool has_null_inputs = !brd || !str;
  int str_size = get_str_lng(str);
  bool is_invalid_str_sz = str_size == 0 || str_size % 2 == 1 || str_size > pow(MAX, 2);

  // for a square, result must be an integer
  double brb_sz = sqrt((double)get_str_lng(str));
  is_invalid_str_sz = is_invalid_str_sz || !check_is_int(brb_sz);

  if (has_null_inputs || is_invalid_str_sz)
  {
    return false;
  }

  brd->sz = (int)brb_sz;

  // write characters into respective cells
  for (int i = 0; i < brd->sz; i++)
  {
    for (int j = 0; j < brd->sz; j++)
    {
      int str_idx = (i * brd->sz) + j;
      // make sure characters are all valid ones
      char val = str[str_idx];
      bool is_valid_char = validate_char(val);
      if (!is_valid_char)
      {
        return false;
      }
      brd->b2d[i][j] = str[str_idx];
    }
  }
  return true;
}

void board2str(char *str, board *brd)
{
  for (int i = 0; i < brd->sz; i++)
  {
    for (int j = 0; j < brd->sz; j++)
    {
      str[i * brd->sz + j] = brd->b2d[i][j];
    }
  }
  int last_inx = (int)pow(brd->sz, 2);
  str[last_inx] = '\0';
}

bool solve_board(board *brd)
{
  if (!brd)
  {
    return false;
  }

  // used to check if any cells have been filled by the rules
  bool is_apply_pairs, is_apply_oxo, is_apply_counting, is_valid_board;
  do
  {
    // reset the status
    is_apply_pairs = false;
    is_apply_oxo = false;

    // make sure it still meets basic requirements
    is_valid_board = validate_grid(brd);
    if (!is_valid_board)
    {
      return false;
    }

    for (int i = 0; i < brd->sz; i++)
    {
      for (int j = 0; j < brd->sz; j++)
      {
        apply_pairs(brd, i, j) && (is_apply_pairs = true);
        apply_oxo(brd, i, j) && (is_apply_oxo = true);
      }
    }

    is_apply_counting = apply_counting(brd);
  } while (is_apply_pairs || is_apply_oxo || is_apply_counting);
  return check_grid_finished(brd);
}

bool apply_pairs(board *brd, int i, int j)
{
  bool should_apply2_row = j < brd->sz - 1;
  bool should_apply2_col = i < brd->sz - 1;

  bool is_row_applied = should_apply2_row && apply_pairs2_target(brd, i, j, row);
  bool is_col_applied = should_apply2_col && apply_pairs2_target(brd, i, j, column);

  return is_row_applied || is_col_applied;
}

bool apply_pairs2_target(board *brd, int i, int j, target t)
{
  bool is_applied = false;

  int next_i = t ? i + 1 : i;
  int next_next_i = t ? i + 2 : i;
  int next_j = t ? j : j + 1;
  int next_next_j = t ? j : j + 2;
  int prev_i = t ? i - 1 : i;
  int prev_j = t ? j : j - 1;

  int current_val = brd->b2d[i][j];
  int opposite_val = get_opposite_val(current_val);
  bool has_val = current_val != UNK;
  bool have2_same_val = current_val == brd->b2d[next_i][next_j];
  bool is_next2_in_grid = next_next_i < brd->sz && next_next_j < brd->sz;
  bool is_next2_empty = brd->b2d[next_next_i][next_next_j] == UNK;
  bool is_prev_in_grid = prev_i >= 0 && prev_j >= 0;
  bool is_prev_empty = brd->b2d[prev_i][prev_j] == UNK;
  bool is_match_rule = has_val && have2_same_val;

  // Apply value to the next next cell
  if (is_match_rule && is_next2_in_grid && is_next2_empty)
  {
    is_applied = true;
    brd->b2d[next_next_i][next_next_j] = opposite_val;
  }
  // Apply value to the previous cell
  if (is_match_rule && is_prev_in_grid && is_prev_empty)
  {
    is_applied = true;
    brd->b2d[prev_i][prev_j] = opposite_val;
  }

  return is_applied;
}

bool apply_oxo(board *brd, int i, int j)
{
  bool is_row_applied = false;
  bool is_col_applied = false;
  int current_val = brd->b2d[i][j];
  bool has_val = cell_has_val(current_val);

  if (!has_val)
  {
    return false;
  }

  // apply to rows
  if (j < brd->sz - 2)
  {
    is_row_applied = apply_oxo2_target(brd, i, j, row);
  }

  // apply to columns
  if (i < brd->sz - 2)
  {
    is_col_applied = apply_oxo2_target(brd, i, j, column);
  }

  return is_row_applied || is_col_applied;
}

bool apply_oxo2_target(board *brd, int i, int j, target t)
{
  bool is_applied = false;

  int next_i = t ? i + 1 : i;
  int next_next_i = t ? i + 2 : i;
  int next_j = t ? j : j + 1;
  int next_next_j = t ? j : j + 2;

  int current_val = brd->b2d[i][j];
  int opposite_val = get_opposite_val(current_val);
  int next2_val = brd->b2d[next_next_i][next_next_j];
  bool have2_same_val = current_val == next2_val;
  int next_val = brd->b2d[next_i][next_j];
  bool is_next_empty = !cell_has_val(next_val);

  if (have2_same_val && is_next_empty)
  {
    is_applied = true;
    brd->b2d[next_i][next_j] = opposite_val;
  }
  return is_applied;
}

bool apply_counting(board *brd)
{
  bool is_applied = false;

  for (int i = 0; i < brd->sz; i++)
  {
    int row_zero_cnt = 0;
    int row_one_cnt = 0;
    int col_zero_cnt = 0;
    int col_one_cnt = 0;

    for (int j = 0; j < brd->sz; j++)
    {
      // Count row cells
      int current_val = brd->b2d[i][j];
      increment_val(current_val, &row_zero_cnt, &row_one_cnt);

      // Count column cells
      current_val = brd->b2d[j][i];
      increment_val(current_val, &col_zero_cnt, &col_one_cnt);
    }

    // Apply to row
    bool is_r_applied = apply_val2_target(brd, i, row_zero_cnt, row_one_cnt, row);

    // Apply to column
    bool is_c_applied = apply_val2_target(brd, i, col_zero_cnt, col_one_cnt, column);

    (is_r_applied || is_c_applied) && (is_applied = true);
  }
  return is_applied;
}

void increment_val(char v, int *z_cnt, int *o_cnt)
{
  if (v == ONE)
  {
    *o_cnt = *o_cnt + 1;
  }
  else if (v == ZERO)
  {
    *z_cnt = *z_cnt + 1;
  }
  else
  {
    return;
  }
}

int get_str_lng(char s[])
{
  if (s == NULL)
  {
    return false;
  }
  int index = 0;
  while (s[index])
  {
    index++;
  }
  return index;
}

char get_opposite_val(char c)
{
  switch (c)
  {
  case ONE:
    return ZERO;
  case ZERO:
    return ONE;
  default:
    return UNK;
  }
}

bool validate_grid(board *brd)
{
  for (int i = 0; i < brd->sz; i++)
  {
    for (int j = 0; j < brd->sz; j++)
    {
      int current_val = brd->b2d[i][j];
      bool is_filled_cell = cell_has_val(current_val);
      bool should_validate_row = j < brd->sz - 2;
      bool should_validate_col = i < brd->sz - 2;

      int next_col = brd->b2d[i][j + 1];
      int next_next_col = brd->b2d[i][j + 2];

      int next_row = brd->b2d[i + 1][j];
      int next_next_row = brd->b2d[i + 2][j];

      // not more than 2 consecutive nums in a row
      bool has3same_col_nums = (current_val == next_col) && (next_col == next_next_col);
      bool is_invalid_row = should_validate_row && is_filled_cell && has3same_col_nums;

      // not more than 2 consecutive nums in a column
      bool has3same_row_nums = (current_val == next_row) && (next_row == next_next_row);
      bool is_invalid_col = should_validate_col && is_filled_cell && has3same_row_nums;

      if (is_invalid_row || is_invalid_col)
      {
        return false;
      }
    }
  }
  return true;
}

bool check_grid_finished(board *brd)
{
  for (int i = 0; i < brd->sz; i++)
  {
    for (int j = 0; j < brd->sz; j++)
    {
      if (!cell_has_val(brd->b2d[i][j]))
      {
        return false;
      }
    }
  }
  return true;
}

bool apply_val2_target(board *brd, int i, int zero_cnt, int one_cnt, target t)
{
  int half_sz = brd->sz / 2;
  bool is_zero_completed = zero_cnt == half_sz;
  bool is_one_completed = one_cnt == half_sz;
  bool should_apply = is_zero_completed ^ is_one_completed;

  if (!should_apply)
  {
    return false;
  }

  char completed_val = zero_cnt == half_sz ? ZERO : ONE;

  for (int j = 0; j < brd->sz; j++)
  {
    // apply value to the cell in a specific row
    if (t == 0 && !cell_has_val(brd->b2d[i][j]))
    {
      brd->b2d[i][j] = get_opposite_val(completed_val);
    }
    // apply value to the cell in a specific column
    else if (t == 1 && !cell_has_val(brd->b2d[j][i]))
    {
      brd->b2d[j][i] = get_opposite_val(completed_val);
    }
  }

  return true;
}

bool check_is_int(float f)
{
  int i = f;
  double r = f - i;
  return !(r > 0);
}

void printboard(board *brd)
{
  for (int i = 0; i < brd->sz; i++)
  {
    for (int j = 0; j < brd->sz; j++)
    {
      printf("%c ", brd->b2d[i][j]);
    }
    printf("\n");
  }
}

bool validate_char(char c)
{
  switch (c)
  {
  case ONE:
  case ZERO:
  case UNK:
    return true;
  default:
    return false;
  }
}

bool cell_has_val(char c)
{
  switch (c)
  {
  case ONE:
  case ZERO:
    return true;
  default:
    return false;
  }
}

void test(void)
{
  board b;

  char test_s1[] = "123";
  char test_s2[] = "";

  int test_zero_cnt = 0;
  int test_one_cnt = 0;

  assert(get_str_lng(NULL) == 0);
  assert(get_str_lng(test_s1) == 3);
  assert(get_str_lng(test_s2) == 0);

  assert(validate_char(ONE) == true);
  assert(validate_char(ZERO) == true);
  assert(validate_char(UNK) == true);
  assert(validate_char('X') == false);

  assert(cell_has_val(ONE) == true);
  assert(cell_has_val(ZERO) == true);
  assert(cell_has_val(UNK) == false);

  assert(get_opposite_val(ONE) == ZERO);
  assert(get_opposite_val(ZERO) == ONE);

  assert(check_is_int(4) == true);
  assert(check_is_int(3.2) == false);

  // Add one to One
  increment_val(ONE, &test_zero_cnt, &test_one_cnt);
  assert(test_zero_cnt == 0);
  assert(test_one_cnt == 1);

  // Add one to Zero
  increment_val(ZERO, &test_zero_cnt, &test_one_cnt);
  assert(test_zero_cnt == 1);
  assert(test_one_cnt == 1);

  // Invalid input add nothing to One & Zero
  increment_val('x', &test_zero_cnt, &test_one_cnt);
  assert(test_zero_cnt == 1);
  assert(test_one_cnt == 1);

  // Invalid 3x4 Board
  assert(str2board(&b, "...1.0......") == false);

  // Invalid 4x4 Board: Contain an invalid character
  assert(str2board(&b, "..x1.0......1..1") == false);
  assert(str2board(&b, "33.1.0......1..1") == false);

  // Valid 4x4 Board
  str2board(&b, "...1.0......1..1");

  // Valid 4x4 Board
  assert(str2board(&b, "...1.0......1..1") == true);
  assert(validate_grid(&b) == true);

  // Apply value to all empty cells in a row: Not needed
  assert(str2board(&b, "0.1......1.0....") == true);
  assert(validate_grid(&b) == true);
  assert(apply_val2_target(&b, 0, 1, 1, row) == false);

  // Apply value to all empty cells in a column: Not needed
  assert(str2board(&b, "0.1......1.0....") == true);
  assert(validate_grid(&b) == true);
  assert(apply_val2_target(&b, 0, 1, 1, column) == false);

  // Apply value to all empty cells in a row: Succeed
  assert(str2board(&b, "00........11....") == true);
  assert(validate_grid(&b) == true);
  assert(apply_val2_target(&b, 0, 2, 0, row) == true);
  assert(b.b2d[0][2] == ONE);
  assert(b.b2d[0][3] == ONE);
  assert(b.b2d[0][0] == ZERO);
  assert(b.b2d[0][1] == ZERO);

  // Apply value to all empty cells in a column: Succeed
  assert(str2board(&b, "11......1100....") == true);
  assert(validate_grid(&b) == true);
  assert(apply_val2_target(&b, 0, 0, 2, column) == true);
  assert(b.b2d[1][0] == ZERO);
  assert(b.b2d[3][0] == ZERO);
  assert(b.b2d[0][0] == ONE);
  assert(b.b2d[2][0] == ONE);

  // Counting rule: Applied needed
  assert(str2board(&b, "00........11....") == true);
  assert(validate_grid(&b) == true);
  assert(apply_counting(&b) == true);

  // Counting rule: Applied needed
  assert(str2board(&b, "0.0.............") == true);
  assert(validate_grid(&b) == true);
  assert(apply_counting(&b) == true);

  // Counting rule: Applied not needed
  assert(str2board(&b, "0...............") == true);
  assert(validate_grid(&b) == true);
  assert(apply_counting(&b) == false);

  // Finished 4x4 Board
  assert(str2board(&b, "0011110000111100") == true);
  assert(validate_grid(&b) == true);
  assert(check_grid_finished(&b) == true);

  // Unfinished 4x4 Board
  assert(str2board(&b, "0011....0011....") == true);
  assert(validate_grid(&b) == true);
  assert(check_grid_finished(&b) == false);

  // Invalid 4x4 Board: Row
  assert(str2board(&b, "1111.0......1..1") == true);
  assert(validate_grid(&b) == false);

  // Invalid 4x4 Board: Column
  assert(str2board(&b, "1001100110010110") == true);
  assert(validate_grid(&b) == false);

  // Applied One to adjacent cells in a row
  assert(str2board(&b, ".00..11.........") == true);
  assert(validate_grid(&b) == true);
  assert(apply_pairs2_target(&b, 0, 1, row) == true);
  assert(b.b2d[0][3] == ONE);
  assert(b.b2d[0][0] == ONE);

  // Applied Zero to adjacent cells in a column
  assert(str2board(&b, ".....11..1......") == true);
  assert(validate_grid(&b) == true);
  assert(apply_pairs2_target(&b, 1, 1, column) == true);
  assert(b.b2d[0][1] == ZERO);
  assert(b.b2d[3][1] == ZERO);

  // Pairs rule: Empty cell should be skipped
  assert(str2board(&b, "00...11.........") == true);
  assert(validate_grid(&b) == true);
  assert(apply_pairs(&b, 0, 3) == false);

  // Pairs rule: Empty cell should be skipped
  assert(str2board(&b, "00...11.........") == true);
  assert(validate_grid(&b) == true);
  assert(apply_pairs(&b, 3, 0) == false);

  // Pairs rule: Applied to row succeed
  assert(str2board(&b, "00...11.........") == true);
  assert(validate_grid(&b) == true);
  assert(apply_pairs(&b, 0, 0) == true);

  // Pairs rule: Applied not needed
  assert(str2board(&b, "0..1.01..10.1..0") == true);
  assert(validate_grid(&b) == true);
  assert(apply_pairs(&b, 2, 2) == false);

  // OXO rule: Applied once
  assert(str2board(&b, "0.0..1.......1..") == true);
  assert(validate_grid(&b) == true);
  assert(apply_oxo(&b, 0, 0) == true);

  // OXO rule: Applied not needed
  assert(str2board(&b, "0..0.1..1..1..1.") == true);
  assert(validate_grid(&b) == true);
  assert(apply_oxo(&b, 0, 0) == false);

  // OXO rule: skip current index
  assert(str2board(&b, "0.0..1.......1..") == true);
  assert(validate_grid(&b) == true);
  assert(apply_oxo(&b, 0, 3) == false);

  // OXO rule: skip current index
  assert(str2board(&b, "0.0..1.......1..") == true);
  assert(validate_grid(&b) == true);
  assert(apply_oxo(&b, 3, 0) == false);
}
