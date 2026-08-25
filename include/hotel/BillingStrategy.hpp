#pragma once

namespace hotel {

class BillingStrategy {
public:
    virtual ~BillingStrategy() = default;

    /**
     * @param nights          number of nights stayed
     * @param nightlyRate     the room category's base nightly rate
     * @param serviceCharges  extra charges accumulated during the stay
     */
    virtual double calculateBill(int nights, double nightlyRate, double serviceCharges) const = 0;
};

class StandardBillingStrategy : public BillingStrategy {
public:
    double calculateBill(int nights, double nightlyRate, double serviceCharges) const override;
};

class WeekendSurchargeBillingStrategy : public BillingStrategy {
public:
    explicit WeekendSurchargeBillingStrategy(double surchargeMultiplier = 1.2);

    double calculateBill(int nights, double nightlyRate, double serviceCharges) const override;

private:
    double surchargeMultiplier_;
};

} 