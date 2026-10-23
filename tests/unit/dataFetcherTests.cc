#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <optional>
#include <stdexcept>
#include <string>

#include "dataFetcher.h"
#include "mockPriceProvider.h"

namespace {

class mockDaySaver : public daySaver {
public:
    MOCK_METHOD(void, store, (const fetchedDay&), (override));
};

class dataFetcherTest : public testing::Test {
protected:
    const std::chrono::year_month_day day = std::chrono::year{2026} / 9 / 30;
    testing::StrictMock<mockPriceProvider> provider;
    testing::StrictMock<mockDaySaver> writer;
    dataFetcher fetcher{provider, writer};

    energyPricesTable fullDay() const {
        energyPricesTable data{day, {}};
        // An ordinary day; this fixture does not define DST behaviour.
        for (int hour = 1; hour <= 24; ++hour) {
            data.second.push_back({"2026-09-30_H" + std::to_string(hour),
                100.0, 200.0, 300.0});
        }
        return data;
    }
};

TEST_F(dataFetcherTest, StoresRequestedDayAndMeanPriceInOriginalUnits) {
    const energyPricesTable data{day, {{"2026-09-30_H1", 100.0, 200.0, 345.67}}};
    testing::InSequence sequence;
    EXPECT_CALL(provider, getPrices(day)).WillOnce(testing::Return(data));
    EXPECT_CALL(writer, store(testing::_)).WillOnce([&](const fetchedDay& saved) {
        EXPECT_EQ(saved.date, day);
        ASSERT_EQ(saved.prices.size(), 1u);
        EXPECT_EQ(saved.prices[0].period, "2026-09-30_H1");
        EXPECT_DOUBLE_EQ(saved.prices[0].pricePerMWh, 345.67);
        EXPECT_FALSE(saved.completed);
    });

    fetcher.store(day);
}

TEST_F(dataFetcherTest, FallsBackToFixing2ThenFixing1) {
    const energyPricesTable data{day, {
        {"2026-09-30_H1", 100.0, 200.0, std::nullopt},
        {"2026-09-30_H2", 123.45, std::nullopt, std::nullopt}}};
    EXPECT_CALL(provider, getPrices(day)).WillOnce(testing::Return(data));
    EXPECT_CALL(writer, store(testing::_)).WillOnce([](const fetchedDay& saved) {
        EXPECT_THAT(saved.prices, testing::ElementsAre(
            fetchedPrice{"2026-09-30_H1", 200.0},
            fetchedPrice{"2026-09-30_H2", 123.45}));
        EXPECT_FALSE(saved.completed);
    });

    fetcher.store(day);
}

TEST_F(dataFetcherTest, OmitsPeriodWithoutAnyPrice) {
    const energyPricesTable data{day, {
        {"2026-09-30_H1", std::nullopt, std::nullopt, std::nullopt},
        {"2026-09-30_H2", 100.0, 200.0, 300.0}}};
    EXPECT_CALL(provider, getPrices(day)).WillOnce(testing::Return(data));
    EXPECT_CALL(writer, store(testing::_)).WillOnce([](const fetchedDay& saved) {
        EXPECT_THAT(saved.prices, testing::ElementsAre(
            fetchedPrice{"2026-09-30_H2", 300.0}));
        EXPECT_FALSE(saved.completed);
    });

    fetcher.store(day);
}

TEST_F(dataFetcherTest, PreservesZeroAndNegativePrices) {
    const energyPricesTable data{day, {
        {"2026-09-30_H1", 100.0, 200.0, 0.0},
        {"2026-09-30_H2", 100.0, 200.0, -50.5},
        {"2026-09-30_H3", 100.0, 0.0, std::nullopt},
        {"2026-09-30_H4", 0.0, std::nullopt, std::nullopt}}};
    EXPECT_CALL(provider, getPrices(day)).WillOnce(testing::Return(data));
    EXPECT_CALL(writer, store(testing::_)).WillOnce([](const fetchedDay& saved) {
        EXPECT_THAT(saved.prices, testing::ElementsAre(
            fetchedPrice{"2026-09-30_H1", 0.0},
            fetchedPrice{"2026-09-30_H2", -50.5},
            fetchedPrice{"2026-09-30_H3", 0.0},
            fetchedPrice{"2026-09-30_H4", 0.0}));
    });

    fetcher.store(day);
}

TEST_F(dataFetcherTest, MarksOrdinaryDayCompleteWhenEveryPeriodHasMeanPrice) {
    const auto data = fullDay();
    EXPECT_CALL(provider, getPrices(day)).WillOnce(testing::Return(data));
    EXPECT_CALL(writer, store(testing::_)).WillOnce([&](const fetchedDay& saved) {
        EXPECT_EQ(saved.date, day);
        EXPECT_TRUE(saved.completed);
        ASSERT_EQ(saved.prices.size(), data.second.size());
        for (std::size_t i = 0; i < data.second.size(); ++i) {
            EXPECT_EQ(saved.prices[i].period, data.second[i].time);
            EXPECT_DOUBLE_EQ(saved.prices[i].pricePerMWh, *data.second[i].meanPrice);
        }
    });

    fetcher.store(day);
}

TEST_F(dataFetcherTest, MissingMeanKeepsDayIncompleteDespiteAllSelectedPrices) {
    auto data = fullDay();
    data.second.back().meanPrice.reset();
    EXPECT_CALL(provider, getPrices(day)).WillOnce(testing::Return(data));
    EXPECT_CALL(writer, store(testing::_)).WillOnce([](const fetchedDay& saved) {
        EXPECT_FALSE(saved.completed);
        ASSERT_EQ(saved.prices.size(), 24u);
        EXPECT_DOUBLE_EQ(saved.prices.back().pricePerMWh, 200.0);
    });

    fetcher.store(day);
}

TEST_F(dataFetcherTest, MissingPeriodKeepsDayIncompleteDespiteAllRemainingMeans) {
    auto data = fullDay();
    data.second.pop_back();
    EXPECT_CALL(provider, getPrices(day)).WillOnce(testing::Return(data));
    EXPECT_CALL(writer, store(testing::_)).WillOnce([](const fetchedDay& saved) {
        EXPECT_FALSE(saved.completed);
        EXPECT_EQ(saved.prices.size(), 23u);
    });

    fetcher.store(day);
}

TEST_F(dataFetcherTest, RejectsMismatchedDeliveryDateWithoutWriting) {
    auto data = fullDay();
    data.first = std::chrono::year{2026} / 9 / 29;
    EXPECT_CALL(provider, getPrices(day)).WillOnce(testing::Return(data));
    EXPECT_CALL(writer, store(testing::_)).Times(0);

    // The exception type is not yet part of the fetcher's contract.
    EXPECT_ANY_THROW(fetcher.store(day));
}

TEST_F(dataFetcherTest, PropagatesProviderFailureWithoutWriting) {
    EXPECT_CALL(provider, getPrices(day))
        .WillOnce(testing::Throw(std::runtime_error{"provider failed"}));
    EXPECT_CALL(writer, store(testing::_)).Times(0);

    EXPECT_THROW(fetcher.store(day), std::runtime_error);
}

TEST_F(dataFetcherTest, PropagatesWriterFailure) {
    EXPECT_CALL(provider, getPrices(day)).WillOnce(testing::Return(fullDay()));
    EXPECT_CALL(writer, store(testing::_))
        .WillOnce(testing::Throw(std::runtime_error{"writer failed"}));

    EXPECT_THROW(fetcher.store(day), std::runtime_error);
}

TEST_F(dataFetcherTest, DoesNotWriteEmptyResponse) {
    EXPECT_CALL(provider, getPrices(day)).WillOnce(testing::Return(energyPricesTable{day, {}}));
    EXPECT_CALL(writer, store(testing::_)).Times(0);

    EXPECT_NO_THROW(fetcher.store(day));
}

TEST_F(dataFetcherTest, DoesNotWriteWhenAllPricesAreMissing) {
    const energyPricesTable data{day, {
        {"2026-09-30_H1", std::nullopt, std::nullopt, std::nullopt}}};
    EXPECT_CALL(provider, getPrices(day)).WillOnce(testing::Return(data));
    EXPECT_CALL(writer, store(testing::_)).Times(0);

    EXPECT_NO_THROW(fetcher.store(day));
}

TEST_F(dataFetcherTest, RecordsTimestampAfterFetching) {
    std::chrono::system_clock::time_point fetchedAt;
    EXPECT_CALL(provider, getPrices(day)).WillOnce([&](auto) {
        fetchedAt = std::chrono::system_clock::now();
        return fullDay();
    });
    EXPECT_CALL(writer, store(testing::_)).WillOnce([&](const fetchedDay& saved) {
        EXPECT_GE(saved.timestamp, fetchedAt);
        EXPECT_LE(saved.timestamp, std::chrono::system_clock::now());
    });

    fetcher.store(day);
}

// Pending contract: DST period identifiers.
// An exact timestamp test also needs a controllable clock.

} // namespace
