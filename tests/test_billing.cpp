#include <gtest/gtest.h>
#include "hotel/BillingStrategy.hpp"

using hotel::StandardBillingStrategy;
using hotel::WeekendSurchargeBillingStrategy;

TEST(StandardBillingTest, CalculatesBasicBillCorrectly) {
    StandardBillingStrategy strategy;

    double bill = strategy.calculateBill(3, 2000.0, 0.0);
    EXPECT_DOUBLE_EQ(bill, 6000.0);
}

TEST(StandardBillingTest, IncludesServiceCharges) {
    StandardBillingStrategy strategy;

    double bill = strategy.calculateBill(2, 3000.0, 500.0);
    EXPECT_DOUBLE_EQ(bill, 6500.0); // (2*3000) + 500
}

TEST(StandardBillingTest, ZeroNightsWithOnlyServiceCharges) {
    StandardBillingStrategy strategy;

    double bill = strategy.calculateBill(0, 3000.0, 750.0);
    EXPECT_DOUBLE_EQ(bill, 750.0);
}

TEST(WeekendSurchargeBillingTest, AppliesDefaultMultiplierCorrectly) {
    WeekendSurchargeBillingStrategy strategy; // default 1.2x

    double bill = strategy.calculateBill(2, 2000.0, 0.0);
    EXPECT_DOUBLE_EQ(bill, 4800.0); 
}

TEST(WeekendSurchargeBillingTest, AppliesCustomMultiplierCorrectly) {
    WeekendSurchargeBillingStrategy strategy(1.5);

    double bill = strategy.calculateBill(2, 2000.0, 0.0);
    EXPECT_DOUBLE_EQ(bill, 6000.0);
}

TEST(WeekendSurchargeBillingTest, IncludesServiceChargesAfterSurcharge) {
    WeekendSurchargeBillingStrategy strategy(1.2);

    double bill = strategy.calculateBill(2, 2000.0, 400.0);
    EXPECT_DOUBLE_EQ(bill, 5200.0); 
}

TEST(BillingStrategyTest, DifferentStrategiesGiveDifferentResultsForSameStay) {
    StandardBillingStrategy standard;
    WeekendSurchargeBillingStrategy weekend;

    double standardBill = standard.calculateBill(3, 2000.0, 0.0);
    double weekendBill = weekend.calculateBill(3, 2000.0, 0.0);

    EXPECT_NE(standardBill, weekendBill);
    EXPECT_GT(weekendBill, standardBill); 
}