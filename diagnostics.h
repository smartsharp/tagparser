#ifndef TAGPARSER_DIAGNOSTICS_H
#define TAGPARSER_DIAGNOSTICS_H

#include "./global.h"

#include <c++utilities/chrono/datetime.h>

#include <string>
#include <vector>

namespace TagParser {

/*!
 * \brief Specifies the level of the diagnostic message.
 */
enum class DiagLevel {
    None = 0, /**< indicates that no diagnostic messages are present; should not be used when constructing a diagnostic message */
    Debug = 1, /**< indicates a debbuging message */
    Information = 2, /**< indicates an informal message */
    Warning = 3, /**< indicates a warning */
    Critical = 4, /**< indicates a critical error */
    Fatal = 5, /**< indicates a fatal error (note that this level is currently not used) */
};

/// \brief The worst diag level.
constexpr auto worstDiagLevel = DiagLevel::Fatal;

TAG_PARSER_EXPORT const char *diagLevelName(DiagLevel diagLevel);

/*!
 * \brief Sets \a lhs to \a rhs if \a rhs is more critical than \a lhs and returns \a lhs.
 */
inline DiagLevel &operator|=(DiagLevel &lhs, const DiagLevel &rhs)
{
    if (lhs < rhs) {
        lhs = rhs;
    }
    return lhs;
}

class DiagMessage {
public:
    DiagMessage(DiagLevel level, const std::string &message, const std::string &context);
    DiagMessage(DiagLevel level, std::string &&message, const std::string &context);
    DiagMessage(DiagLevel level, const std::string &message, std::string &&context);
    DiagMessage(DiagLevel level, std::string &&message, std::string &&context);

    DiagLevel level() const;
    const char *levelName() const;
    const std::string &message() const;
    const std::string &context() const;
    const ChronoUtilities::DateTime &creationTime() const;
    bool operator==(const DiagMessage &other) const;

    static std::string formatList(const std::vector<std::string> &values);

private:
    DiagLevel m_level;
    std::string m_message;
    std::string m_context;
    ChronoUtilities::DateTime m_creationTime;
};

/*!
 * \brief Constructs a new DiagMessage.
 */
inline DiagMessage::DiagMessage(DiagLevel level, const std::string &message, const std::string &context)
    : m_level(level)
    , m_message(message)
    , m_context(context)
    , m_creationTime(ChronoUtilities::DateTime::gmtNow())
{
}

/*!
 * \brief Constructs a new DiagMessage.
 */
inline DiagMessage::DiagMessage(DiagLevel level, std::string &&message, const std::string &context)
    : m_level(level)
    , m_message(message)
    , m_context(context)
    , m_creationTime(ChronoUtilities::DateTime::gmtNow())
{
}

/*!
 * \brief Constructs a new DiagMessage.
 */
inline DiagMessage::DiagMessage(DiagLevel level, const std::string &message, std::string &&context)
    : m_level(level)
    , m_message(message)
    , m_context(context)
    , m_creationTime(ChronoUtilities::DateTime::gmtNow())
{
}

/*!
 * \brief Constructs a new DiagMessage.
 */
inline DiagMessage::DiagMessage(DiagLevel level, std::string &&message, std::string &&context)
    : m_level(level)
    , m_message(message)
    , m_context(context)
    , m_creationTime(ChronoUtilities::DateTime::gmtNow())
{
}

/*!
 * \brief Returns the level.
 */
inline DiagLevel DiagMessage::level() const
{
    return m_level;
}

/*!
 * \brief Returns the string representation of the level().
 */
inline const char *DiagMessage::levelName() const
{
    return diagLevelName(m_level);
}

/*!
 * \brief Returns the message.
 */
inline const std::string &DiagMessage::message() const
{
    return m_message;
}

/*!
 * \brief Returns the context.
 */
inline const std::string &DiagMessage::context() const
{
    return m_context;
}

/*!
 * \brief Returns the creation time (using GMT timezone).
 */
inline const ChronoUtilities::DateTime &DiagMessage::creationTime() const
{
    return m_creationTime;
}

/*!
 * \brief Returns whether the current instance equals \a other. Everything but the creationTime() is considered.
 */
inline bool DiagMessage::operator==(const DiagMessage &other) const
{
    return m_level == other.m_level && m_message == other.m_message && m_context == other.m_context;
}

class TAG_PARSER_EXPORT Diagnostics : public std::vector<DiagMessage> {
public:
    Diagnostics() = default;
    Diagnostics(std::initializer_list<DiagMessage> list);

    bool has(DiagLevel level) const;
    DiagLevel level() const;
};

/*!
 * \brief Constructs a new container with the specified messages.
 */
inline Diagnostics::Diagnostics(std::initializer_list<DiagMessage> list)
    : std::vector<DiagMessage>(list)
{
}

/*!
 * \brief The DiagPtr class is a leftover from development which should have been removed.
 * \deprecated Don't use, this should never have been committed in the first place.
 * \todo Remove in v8.
 */
class DiagPtr {
public:
    DiagPtr();
    DiagPtr(Diagnostics &diag);
    Diagnostics *operator->();
    const Diagnostics *operator->() const;

private:
    Diagnostics *m_ptr;
};

inline DiagPtr::DiagPtr()
    : m_ptr(nullptr)
{
}

inline DiagPtr::DiagPtr(Diagnostics &diag)
    : m_ptr(&diag)
{
}

inline Diagnostics *DiagPtr::operator->()
{
    return m_ptr;
}

inline const Diagnostics *DiagPtr::operator->() const
{
    return m_ptr;
}

} // namespace TagParser

#endif // TAGPARSER_DIAGNOSTICS_H
