#ifndef LINBOPOSTPROCESSACTIONS_H
#define LINBOPOSTPROCESSACTIONS_H
#include <QFlags>

class LinboPostProcessActions {
public:
    // Possible workflows:
    // create -> upload -> shutdown/reboot/logout
    // upload ---------´
    //
    // autoPartition -> autoInitCache -> autostart -> logout
    // autoPartition ----------------´------------´
    // autoInitCache ---------------´------------´
    enum Flag {
        NoAction = 1,
        Shutdown = 2,
        Reboot = 4,
        Logout = 8,
        UploadImage = 16,
        ExecuteAutoInitCache = 32,
        ExecuteAutostart = 64
    };
    Q_DECLARE_FLAGS(Flags, Flag)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(LinboPostProcessActions::Flags)
#endif // LINBOPOSTPROCESSACTIONS_H
