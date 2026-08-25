#include "hotel/BillingStrategy.hpp"

namespace hotel {

double StandardBillingStrategy::calculateBill(int nights, double nightlyRate, double serviceCharges) const {
    return (nights * nightlyRate) + serviceCharges;
}

WeekendSurchargeBillingStrategy::WeekendSurchargeBillingStrategy(double surchargeMultiplier)
    : surchargeMultiplier_(surchargeMultiplier) {}

double WeekendSurchargeBillingStrategy::calculateBill(int nights, double nightlyRate, double serviceCharges) const {
    return (nights * nightlyRate * surchargeMultiplier_) + serviceCharges;
}

}