#include "RR32Can/StationTxCbk.h"

#include "application/controller/ActionlistStorageCbk.h"
#include "application/controller/MasterControl.h"
#include "application/controller/SettingsStorageCbk.h"
#include "application/controller/TurnoutMapStorageCbk.h"

class SettingsStorageCbkMock : public application::controller::SettingsStorageCbk {
 public:
  MOCK_METHOD(void, store, (const application::model::Settings* const data));
  MOCK_METHOD(bool, load, (application::model::Settings * data));
  MOCK_METHOD(size_t, loadData, (application::model::Settings * data));
};

class TurnoutMapStorageCbkMock : public application::controller::TurnoutMapStorageCbk {
 public:
  MOCK_METHOD(bool, load, (application::model::TurnoutMap & turnoutMap));
  MOCK_METHOD(void, store, (const application::model::TurnoutMap& turnoutMap));
};

class ActionlistStorageCbkMock : public application::controller::ActionlistStorageCbk {
 public:
  MOCK_METHOD(bool, load, (application::model::ActionListModel::DB_t & db));
  MOCK_METHOD(void, store, (const application::model::ActionListModel::DB_t& db));
};

class StationTxCbkMock : public RR32Can::StationTxCbk {
 public:
  MOCK_METHOD(void, SendPacket, (const RR32Can::Identifier&, const RR32Can::Data&));
};

class MainFixture : public ::testing::Test {
 public:
  void SetUp() {
    EXPECT_CALL(masterControl.getInputState().encoder, setPosition(::testing::_));
    EXPECT_CALL(settingsCbk, load(::testing::_));
    EXPECT_CALL(turnoutMapStorageCbk, load(::testing::_));
    EXPECT_CALL(actionListCallback, load(::testing::_));

    masterControl.begin(settingsCbk, turnoutMapStorageCbk, actionListCallback);
    RR32Can::RR32Can.begin(0, masterControl, txCbk);
  };
  void TearDown(){};

  SettingsStorageCbkMock settingsCbk;
  TurnoutMapStorageCbkMock turnoutMapStorageCbk;
  ActionlistStorageCbkMock actionListCallback;

  application::controller::MasterControl masterControl;
  StationTxCbkMock txCbk;
};