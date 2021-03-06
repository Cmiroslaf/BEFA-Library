//
// Created by miro on 16/10/16.
//


#include <gtest/gtest.h>
#include <algorithm>

#include "../include/befa/utils/algorithms.hpp"
#include "fixtures.hpp"

namespace {

class ExecutableFixture : public ::testing::Test {
 protected:
  ExecutableFixture() : file(ExecutableFile::open(file_name)) {}

  virtual ~ExecutableFixture() {}

  ExecutableFile file;
};

TEST_F(ExecutableFixture, FileIsValid) {
  EXPECT_TRUE(file.isValid());
}

TEST_F(ExecutableFixture, FetchSymbols) {
  auto sym_table = file.getSymbolTable();

  EXPECT_FALSE(sym_table.empty());

  EXPECT_NE(sym_table.end(),
            std::find_if(sym_table.begin(), sym_table.end(), [](auto &sym) {
              return ptr_lock(sym)->operator==("main");
            }));
}

TEST_F(ExecutableFixture, FetchSections) {
  auto sections = file.getSections();

  EXPECT_FALSE(sections.empty());

  EXPECT_NE(sections.end(),
            std::find_if(sections.begin(), sections.end(), [](auto &sec) {
              return ptr_lock(sec)->getName() == ".text";
            }));
}

TEST_F(ExecutableFixture, FetchSectionViaSymbols) {
  file.getSymbolTable();
  auto sections = file.getSections();

  EXPECT_FALSE(sections.empty());

  EXPECT_NE(sections.end(),
            std::find_if(sections.begin(), sections.end(), [](auto &sec) {
              return ptr_lock(sec)->getName() == ".text";
            }));
}

TEST_F(ExecutableFixture, FindSymbolsSections) {
  auto symbols = file.getSymbolTable();
  auto sections = file.getSections();

  EXPECT_FALSE(symbols.empty());
  EXPECT_FALSE(sections.empty());

  for_each(symbols, [&sections](auto &sym) {
    EXPECT_NE(sections.end(), std::find_if(
        sections.begin(), sections.end(),
        [&sym](auto &sec) -> bool {
          return ptr_lock(sec)->getOrigin() == ptr_lock(
              ptr_lock(*sym)->getParent()
          )->getOrigin();
        }
    ));
  });
}

TEST_F(ExecutableFixture, TestInstruction) {
  //  ::array_view<uint8_t> bytes,
  //  const std::weak_ptr<BasicBlockT> &parent,
  //  std::string decoded,
  //  bfd_vma address
  ExecutableFile::bb_t::ptr::shared dummy_parent = nullptr;
  auto instruction = ExecutableFile::inst_t::info::type(
      ::array_view<uint8_t>(), dummy_parent, "mov eax, ebx", 500);
  auto arr = instruction.parse();

  arr.element_at(0)
      .subscribe([](const std::string &parsed) {
        EXPECT_EQ(parsed, "mov");
      });

  arr.element_at(1)
      .subscribe([](const std::string &parsed) {
        EXPECT_EQ(parsed, "eax");
      });
  arr.element_at(2)
      .subscribe([](const std::string &parsed) {
        EXPECT_EQ(parsed, "ebx");
      });
}

}  // namespace

