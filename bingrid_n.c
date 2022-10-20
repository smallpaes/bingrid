#include "bingrid.h"

bool apply_pairs(board *brd, int i, int j);
bool apply_oxo(board *brd, int i, int j);
bool apply_counting(board *brd);
void apply_val_to_row(board *brd, int i, char val);
void apply_val_to_col(board *brd, int j, int val);
int get_str_lng(char s[]);
char get_opposite_val(char c);
bool validate_grid(board *brd);
bool validate_str(char s[]);
bool check_grid_finished(board *brd);
bool should_apply_all_val(int zero_cnt, int one_cnt, int sz);
bool check_is_int(float f);

bool str2board(board *brd, char *str)
{
  bool is_invalid_brb = !brd;
  bool is_invalid_str = !str;
  bool is_invalid_str_sz = get_str_lng(str) == 0 || get_str_lng(str) % 2 == 1 || get_str_lng(str) > pow(MAX, 2);

  // result must be an integer for all squares
  double brb_sz = sqrt((double)get_str_lng(str));
  is_invalid_str_sz = is_invalid_str_sz || !check_is_int(brb_sz);

  if (is_invalid_brb || is_invalid_str || is_invalid_str_sz)
  {
    return 0;
  }

  brd->sz = (int)brb_sz;

  for (int i = 0; i < brd->sz; i++)
  {
    for (int j = 0; j < brd->sz; j++)
    {
      // make sure the characters are all valid ones
      char val = str[brd->sz * i + j];
      if (val != ONE && val != ZERO && val != UNK)
      {
        return 0;
      }
      brd->b2d[i][j] = str[brd->sz * i + j];
    }
  }
  return 1;
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
  str[brd->sz * brd->sz] = '\0';
}

bool solve_board(board *brd)
{
  bool is_apply_pairs, is_apply_oxo, is_apply_counting, is_valid_board;

  do
  {
    // make sure it still meets basic requirement
    is_valid_board = validate_grid(brd);
    if (!is_valid_board)
    {
      return false;
    }

    for (int i = 0; i < brd->sz; i++)
    {
      for (int j = 0; j < brd->sz - 2; j++)
      {
        is_apply_pairs = apply_pairs(brd, i, j);
        is_apply_oxo = apply_oxo(brd, i, j);
      }
    }

    is_apply_counting = apply_counting(brd);

  } while (is_apply_pairs || is_apply_oxo || is_apply_counting);
  return check_grid_finished(brd);
}

bool apply_pairs(board *brd, int i, int j)
{
  bool is_applied = false;

  // apply to rows
  if (j < brd->sz - 1)
  {
    int current_val = brd->b2d[i][j];
    int opposite_val = get_opposite_val(current_val);
    if (current_val != '.' && current_val == brd->b2d[i][j + 1])
    {
      if (j > 0 && brd->b2d[i][j - 1] == '.')
      {
        is_applied = true;
        brd->b2d[i][j - 1] = opposite_val;
      }
      if (j + 2 < brd->sz && brd->b2d[i][j + 2] == '.')
      {
        is_applied = true;
        brd->b2d[i][j + 2] = opposite_val;
      }
    }
  }

  // apply to columns
  if (i < brd->sz - 1)
  {
    int current_val = brd->b2d[i][j];
    int opposite_val = get_opposite_val(current_val);
    if (current_val != '.' && current_val == brd->b2d[i + 1][j])
    {
      if (i > 0 && brd->b2d[i - 1][j] == '.')
      {
        is_applied = true;
        brd->b2d[i - 1][j] = opposite_val;
      }
      if (i + 2 < brd->sz && brd->b2d[i + 2][j] == '.')
      {
        is_applied = true;
        brd->b2d[i + 2][j] = opposite_val;
      }
    }
  }

  return is_applied;
}

bool apply_oxo(board *brd, int i, int j)
{
  bool is_applied = false;

  // apply to rows
  if (j < brd->sz - 2)
  {
    int current_val = brd->b2d[i][j];
    int opposite_val = get_opposite_val(current_val);
    if (current_val != UNK && current_val == brd->b2d[i][j + 2])
    {
      if (brd->b2d[i][j + 1] == UNK)
      {
        is_applied = true;
        brd->b2d[i][j + 1] = opposite_val;
      }
    }
  }

  // apply to columns
  if (i < brd->sz - 2)
  {
    int current_val = brd->b2d[i][j];
    int opposite_val = get_opposite_val(current_val);
    if (current_val != UNK && current_val == brd->b2d[i + 2][j])
    {
      if (brd->b2d[i + 1][j] == UNK)
      {
        is_applied = true;
        brd->b2d[i + 1][j] = opposite_val;
      }
    }
  }

  return is_applied;
}

bool apply_counting(board *brd)
{
  bool is_applied = false;

  // apply to rows
  for (int i = 0; i < brd->sz; i++)
  {
    int zero_cnt = 0;
    int one_cnt = 0;

    for (int j = 0; j < brd->sz; j++)
    {
      int current_val = brd->b2d[i][j];
      if (current_val == ONE)
      {
        one_cnt++;
      }
      else if (current_val == ZERO)
      {
        zero_cnt++;
      }
    }

    if (should_apply_all_val(zero_cnt, one_cnt, brd->sz / 2))
    {
      char completed_val = zero_cnt == brd->sz / 2 ? ZERO : ONE;
      apply_val_to_row(brd, i, get_opposite_val(completed_val));
      is_applied = true;
    }
  }

  // apply to columns
  for (int j = 0; j < brd->sz; j++)
  {
    int zero_cnt = 0;
    int one_cnt = 0;

    for (int i = 0; i < brd->sz; i++)
    {
      int current_val = brd->b2d[i][j];
      if (current_val != '.' && current_val == ONE)
      {
        one_cnt++;
      }
      else if (current_val != '.' && current_val == ZERO)
      {
        zero_cnt++;
      }
    }
    if (should_apply_all_val(zero_cnt, one_cnt, brd->sz / 2))
    {
      char completed_val = zero_cnt == brd->sz / 2 ? ZERO : ONE;
      apply_val_to_col(brd, j, get_opposite_val(completed_val));
      is_applied = true;
    }
  }
  return is_applied;
}

void apply_val_to_row(board *brd, int i, char val)
{
  for (int j = 0; j < brd->sz; j++)
  {
    if (brd->b2d[i][j] == '.')
    {
      brd->b2d[i][j] = val;
    }
  }
}

void apply_val_to_col(board *brd, int j, int val)
{
  for (int i = 0; i < brd->sz; i++)
  {
    if (brd->b2d[i][j] == '.')
    {
      brd->b2d[i][j] = val;
    }
  }
}

int get_str_lng(char s[])
{
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
      bool is_filled_cell = current_val != UNK;
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
      if (brd->b2d[i][j] == UNK)
      {
        return false;
      }
    }
  }
  return true;
}

bool should_apply_all_val(int zero_cnt, int one_cnt, int half_sz)
{
  bool is_found_all_val = zero_cnt == half_sz && one_cnt == half_sz;
  bool is_zero_completed = zero_cnt == half_sz;
  bool is_one_completed = one_cnt == half_sz;

  return !is_found_all_val && (is_zero_completed || is_one_completed);
}

bool check_is_int(float f)
{
  int i = f;
  double r = f - i;
  return !(r > 0);
}

void test(void)
{
  board b;

  char test_s1[] = "123";
  char test_s2[] = "";
  assert(get_str_lng(test_s1) == 3);
  assert(get_str_lng(test_s2) == 0);

  assert(get_opposite_val(ONE) == ZERO);
  assert(get_opposite_val(ZERO) == ONE);

  assert(!should_apply_all_val(2, 2, 2));
  assert(should_apply_all_val(2, 0, 2));
  assert(should_apply_all_val(1, 2, 2));

  assert(check_is_int(4) == true);
  assert(check_is_int(3.2) == false);

  // Invalid 3x4 Board
  assert(str2board(&b, "...1.0......") == false);

  // Valid 4x4 Board
  str2board(&b, "...1.0......1..1");

  // Invalid 4x4 Board
  assert(str2board(&b, "33.1.0......1..1") == false);

  // // Valid 4x4 Board
  // assert(str2board(&b, "...1.0......1..1") == true);
  // assert(validate_grid(&b) == true);

  // // TODO: Valid 4x4 Board: test apply to row
  // assert(str2board(&b, "0.0......1.1....") == true);
  // assert(validate_grid(&b) == true);
  // apply_val_to_row(&b, 0, ONE);

  // // TODO: Valid 4x4 Board: test apply to column
  // assert(str2board(&b, ".1.......1......") == true);
  // assert(validate_grid(&b) == true);
  // apply_val_to_col(&b, 1, ZERO);

  // // TODO: Valid 4x4 Board: test apply counting
  // assert(str2board(&b, "00........11....") == true);
  // assert(validate_grid(&b) == true);
  // assert(apply_counting(&b) == true);

  // // TODO: Valid 4x4 Board: test apply counting
  // assert(str2board(&b, "0.0.............") == true);
  // assert(validate_grid(&b) == true);
  // assert(apply_counting(&b) == true);

  // // TODO: Valid 4x4 Board: test apply counting
  // assert(str2board(&b, "0...............") == true);
  // assert(validate_grid(&b) == true);
  // assert(apply_counting(&b) == false);

  // // TODO: Valid finished 4x4 Board
  // assert(str2board(&b, "0011110000111100") == true);
  // assert(validate_grid(&b) == true);
  // assert(check_grid_finished(&b) == true);

  // // TODO: Valid unfinished 4x4 Board
  // assert(str2board(&b, "0011....0011....") == true);
  // assert(validate_grid(&b) == true);
  // assert(check_grid_finished(&b) == false);

  // // Invalid 4x4 Board
  // assert(str2board(&b, "1111.0......1..1") == true);
  // assert(validate_grid(&b) == false);

  // // Pairs rule applied once
  // assert(str2board(&b, "00...11.........") == true);
  // assert(validate_grid(&b) == true);
  // assert(apply_pairs(&b) == true);

  // // Pairs rule applied not needed
  // assert(str2board(&b, "0..1.01..10.1..0") == true);
  // assert(validate_grid(&b) == true);
  // assert(apply_pairs(&b) == false);

  // // OXO rule applied once
  // assert(str2board(&b, "0.0..1.......1..") == true);
  // assert(validate_grid(&b) == true);
  // assert(apply_oxo(&b) == true);

  // // Pairs rule applied not needed
  // assert(str2board(&b, "0..0.1..1..1..1.") == true);
  // assert(validate_grid(&b) == true);
  // assert(apply_oxo(&b) == false);
}
