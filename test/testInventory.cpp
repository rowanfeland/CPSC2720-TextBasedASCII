#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "Inventory.hpp"
#include "Item.hpp"

TEST(ItemTest, MatchesNameOrLastWord) {
  Item cup("tin cup", "A dented tin cup.");
  EXPECT_TRUE(cup.matches("Cup"));
  EXPECT_TRUE(cup.matches("tin cup"));
  EXPECT_FALSE(cup.matches("tin"));
  EXPECT_FALSE(cup.matches("lantern"));
}

TEST(ItemTest, ExamineDefaultsToDescription) {
  Item cup("tin cup", "A dented tin cup.");
  EXPECT_EQ("A dented tin cup.", cup.examine());
  EXPECT_EQ("tin cup", cup.getName());
}

TEST(InventoryTest, AddFindRemove) {
  Inventory inv(3);
  EXPECT_EQ(nullptr, inv.add(std::unique_ptr<Item>(new Item("tin cup", "d"))).get());
  EXPECT_EQ(1, inv.size());
  ASSERT_NE(nullptr, inv.find("cup"));
  EXPECT_EQ("tin cup", inv.find("cup")->getName());
  std::unique_ptr<Item> taken = inv.remove("cup");
  ASSERT_NE(nullptr, taken.get());
  EXPECT_EQ("tin cup", taken->getName());
  EXPECT_EQ(0, inv.size());
  EXPECT_EQ(nullptr, inv.find("cup"));
}

TEST(InventoryTest, AddWhenFullHandsBack) {
  Inventory inv(1);
  inv.add(std::unique_ptr<Item>(new Item("cup", "d")));
  EXPECT_TRUE(inv.isFull());
  std::unique_ptr<Item> rejected =
      inv.add(std::unique_ptr<Item>(new Item("lamp", "d")));
  ASSERT_NE(nullptr, rejected.get());
  EXPECT_EQ("lamp", rejected->getName());
  EXPECT_EQ(1, inv.size());
}

TEST(InventoryTest, RemoveMissingIsNull) {
  Inventory inv(2);
  EXPECT_EQ(nullptr, inv.remove("sword").get());
}

TEST(InventoryTest, ListEmptyAndFull) {
  Inventory inv(2);
  EXPECT_EQ("You are carrying nothing.", inv.list());
  inv.add(std::unique_ptr<Item>(new Item("tin cup", "d")));
  EXPECT_EQ("You are carrying tin cup.", inv.list());
  inv.add(std::unique_ptr<Item>(new Item("lantern", "d")));
  EXPECT_EQ("You are carrying tin cup and lantern.", inv.list());
}
