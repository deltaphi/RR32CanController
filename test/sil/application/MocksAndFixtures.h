#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "RR32Can/callback/TxCbk.h"

#include "application/controller/ActionlistStorageCbk.h"
#include "application/controller/MasterControl.h"
#include "application/controller/SettingsStorageCbk.h"
#include "application/controller/TurnoutMapStorageCbk.h"

class SettingsStorageCbkMock : public application::controller::SettingsStorageCbk {
 public:
  MOCK_METHOD(void, store, (const application::model::Settings* const data), (override));
  MOCK_METHOD(bool, load, (application::model::Settings * data), (override));
  MOCK_METHOD(size_t, loadData, (application::model::Settings * data), (override));
};

class TurnoutMapStorageCbkMock : public application::controller::TurnoutMapStorageCbk {
 public:
  MOCK_METHOD(bool, load, (application::model::TurnoutMap & turnoutMap), (override));
  MOCK_METHOD(void, store, (const application::model::TurnoutMap& turnoutMap), (override));
};

class ActionlistStorageCbkMock : public application::controller::ActionlistStorageCbk {
 public:
  MOCK_METHOD(bool, load, (application::model::ActionListModel::DB_t & db), (override));
  MOCK_METHOD(void, store, (const application::model::ActionListModel::DB_t& db), (override));
};

class StationTxCbkMock : public RR32Can::callback::TxCbk {
 public:
  MOCK_METHOD(void, SendPacket, (const RR32Can::CanFrame&), (override));
};

class MainFixture : public ::testing::Test {
 public:
  void SetUp() {
    EXPECT_CALL(masterControl.getInputState().encoder, setPosition(::testing::_));
    EXPECT_CALL(settingsCbk, load(::testing::_));
    EXPECT_CALL(turnoutMapStorageCbk, load(::testing::_));
    EXPECT_CALL(actionListCallback, load(::testing::_));

    masterControl.begin(settingsCbk, turnoutMapStorageCbk, actionListCallback);
    RR32Can::Station::CallbackStruct callbacks;
    callbacks.system = &masterControl;
    callbacks.engine = &masterControl;
    callbacks.tx = &txCbk;

    RR32Can::RR32Can.begin(0, callbacks);
  };
  void TearDown(){};

  SettingsStorageCbkMock settingsCbk;
  TurnoutMapStorageCbkMock turnoutMapStorageCbk;
  ActionlistStorageCbkMock actionListCallback;

  application::controller::MasterControl masterControl;
  StationTxCbkMock txCbk;
};