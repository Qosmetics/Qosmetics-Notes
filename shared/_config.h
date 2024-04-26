#pragma once

#define QOSMETICS_NOTES_EXPORT __attribute__((visibility("default")))

#if __cplusplus
#define QOSMETICS_NOTES_EXPORT_FUNC extern "C" QOSMETICS_NOTES_EXPORT
#else
#define QOSMETICS_NOTES_EXPORT_FUNC QOSMETICS_NOTES_EXPORT
#endif
