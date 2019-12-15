#include "model/ActionListDB.h"

#include "SPIFFS.h"

#include "model/TurnoutTypes.h"

namespace model {

namespace ActionListDB {

static const char* kActionListFilename = "/actionlist.prefs";

struct FileHeader_t {
  Index_t numLists;
};

struct ListHeader_t {
  Index_t listLength;
};

bool load(DB_t& db) {
  File f = SPIFFS.open(kActionListFilename);

  if (!f) {
    printf("Opening '%s' for reading failed.\n", kActionListFilename);
    return 0;
  } else {
    db.clear();

    size_t readBytes = 0;
    size_t totalReadBytes;
    FileHeader_t fileHeader;
    readBytes =
        f.read(reinterpret_cast<uint8_t*>(&fileHeader), sizeof(fileHeader));
    totalReadBytes += readBytes;

    if (readBytes >= sizeof(fileHeader)) {
      for (int i = 0; i < fileHeader.numLists; ++i) {
        ListHeader_t listHeader;

        readBytes =
            f.read(reinterpret_cast<uint8_t*>(&listHeader), sizeof(listHeader));
        totalReadBytes += readBytes;

        if (readBytes < sizeof(listHeader)) {
          break;
        }
        ActionList_t newList;

        for (int j = 0; j < listHeader.listLength; ++j) {
          TurnoutAction action;

          readBytes =
              f.read(reinterpret_cast<uint8_t*>(&action), sizeof(action));
          totalReadBytes += readBytes;

          if (readBytes < sizeof(action)) {
            break;
          }
          newList.push_back(action);
        }

        db.push_back(std::move(newList));
      }
    }

    f.close();
    printf("%s: Loaded %i bytes.\n", kActionListFilename, readBytes);
    return true;
  }
}

void store(const DB_t& db) {
  // We need to store
  File f = SPIFFS.open(kActionListFilename, "w");

  if (!f) {
    printf("Opening '%s' for writing failed.\n", kActionListFilename);
  } else {
    FileHeader_t fileHeader = {numLists : static_cast<Index_t>(db.size())};

    size_t writtenBytes =
        f.write(reinterpret_cast<uint8_t*>(&fileHeader), sizeof(fileHeader));

    for (const ActionList_t& al : db) {
      ListHeader_t listHeader = {listLength : static_cast<Index_t>(al.size())};

      writtenBytes +=
          f.write(reinterpret_cast<uint8_t*>(&listHeader), sizeof(listHeader));

      for (const TurnoutAction& action : al) {
        writtenBytes +=
            f.write(reinterpret_cast<const uint8_t*>(&action), sizeof(action));
      }
    }
    printf("%s: Wrote %i bytes.\n", kActionListFilename, writtenBytes);
  }

  f.close();
}

}  // namespace ActionListDB

}  // namespace model
