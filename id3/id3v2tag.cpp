#include "./id3v2tag.h"
#include "./id3v2frameids.h"

#include "../diagnostics.h"
#include "../exceptions.h"

#include <c++utilities/conversion/stringbuilder.h>
#include <c++utilities/conversion/stringconversion.h>

#include <iostream>

using namespace std;
using namespace IoUtilities;
using namespace ConversionUtilities;

namespace TagParser {

/*!
 * \class TagParser::Id3v2Tag
 * \brief Implementation of TagParser::Tag for ID3v2 tags.
 */

Id3v2Tag::IdentifierType Id3v2Tag::internallyGetFieldId(KnownField field) const
{
    using namespace Id3v2FrameIds;
    if (m_majorVersion >= 3) {
        switch (field) {
        case KnownField::Album:
            return lAlbum;
        case KnownField::Artist:
            return lArtist;
        case KnownField::Comment:
            return lComment;
        case KnownField::Year:
            return lYear;
        case KnownField::RecordDate:
            return lRecordDate;
        case KnownField::Title:
            return lTitle;
        case KnownField::Genre:
            return lGenre;
        case KnownField::TrackPosition:
            return lTrackPosition;
        case KnownField::DiskPosition:
            return lDiskPosition;
        case KnownField::Encoder:
            return lEncoder;
        case KnownField::Bpm:
            return lBpm;
        case KnownField::Cover:
            return lCover;
        case KnownField::Lyricist:
            return lWriter;
        case KnownField::Length:
            return lLength;
        case KnownField::Language:
            return lLanguage;
        case KnownField::EncoderSettings:
            return lEncoderSettings;
        case KnownField::Lyrics:
            return lUnsynchronizedLyrics;
        case KnownField::SynchronizedLyrics:
            return lSynchronizedLyrics;
        case KnownField::Grouping:
            return lGrouping;
        case KnownField::RecordLabel:
            return lRecordLabel;
        case KnownField::Composer:
            return lComposer;
        case KnownField::Rating:
            return lRating;
        default:;
        }
    } else {
        switch (field) {
        case KnownField::Album:
            return sAlbum;
        case KnownField::Artist:
            return sArtist;
        case KnownField::Comment:
            return sComment;
        case KnownField::Year:
            return sYear;
        case KnownField::RecordDate:
            return sRecordDate;
        case KnownField::Title:
            return sTitle;
        case KnownField::Genre:
            return sGenre;
        case KnownField::TrackPosition:
            return sTrackPosition;
        case KnownField::DiskPosition:
            return sDiskPosition;
        case KnownField::Encoder:
            return sEncoder;
        case KnownField::Bpm:
            return sBpm;
        case KnownField::Cover:
            return sCover;
        case KnownField::Lyricist:
            return sWriter;
        case KnownField::Length:
            return sLength;
        case KnownField::Language:
            return sLanguage;
        case KnownField::EncoderSettings:
            return sEncoderSettings;
        case KnownField::Lyrics:
            return sUnsynchronizedLyrics;
        case KnownField::SynchronizedLyrics:
            return sSynchronizedLyrics;
        case KnownField::Grouping:
            return sGrouping;
        case KnownField::RecordLabel:
            return sRecordLabel;
        case KnownField::Composer:
            return sComposer;
        case KnownField::Rating:
            return sRating;
        default:;
        }
    }
    return 0;
}

KnownField Id3v2Tag::internallyGetKnownField(const IdentifierType &id) const
{
    using namespace Id3v2FrameIds;
    switch (id) {
    case lAlbum:
        return KnownField::Album;
    case lArtist:
        return KnownField::Artist;
    case lComment:
        return KnownField::Comment;
    case lYear:
        return KnownField::Year;
    case lRecordDate:
        return KnownField::RecordDate;
    case lTitle:
        return KnownField::Title;
    case lGenre:
        return KnownField::Genre;
    case lTrackPosition:
        return KnownField::TrackPosition;
    case lDiskPosition:
        return KnownField::DiskPosition;
    case lEncoder:
        return KnownField::Encoder;
    case lBpm:
        return KnownField::Bpm;
    case lCover:
        return KnownField::Cover;
    case lWriter:
        return KnownField::Lyricist;
    case lLanguage:
        return KnownField::Language;
    case lLength:
        return KnownField::Length;
    case lEncoderSettings:
        return KnownField::EncoderSettings;
    case lUnsynchronizedLyrics:
        return KnownField::Lyrics;
    case lSynchronizedLyrics:
        return KnownField::SynchronizedLyrics;
    case lGrouping:
        return KnownField::Grouping;
    case lRecordLabel:
        return KnownField::RecordLabel;
    case sAlbum:
        return KnownField::Album;
    case sArtist:
        return KnownField::Artist;
    case sComment:
        return KnownField::Comment;
    case sYear:
        return KnownField::Year;
    case sRecordDate:
        return KnownField::RecordDate;
    case sTitle:
        return KnownField::Title;
    case sGenre:
        return KnownField::Genre;
    case sTrackPosition:
        return KnownField::TrackPosition;
    case sEncoder:
        return KnownField::Encoder;
    case sBpm:
        return KnownField::Bpm;
    case sCover:
        return KnownField::Cover;
    case sWriter:
        return KnownField::Lyricist;
    case sLanguage:
        return KnownField::Language;
    case sLength:
        return KnownField::Length;
    case sEncoderSettings:
        return KnownField::EncoderSettings;
    case sUnsynchronizedLyrics:
        return KnownField::Lyrics;
    case sSynchronizedLyrics:
        return KnownField::SynchronizedLyrics;
    case sGrouping:
        return KnownField::Grouping;
    case sRecordLabel:
        return KnownField::RecordLabel;
    default:
        return KnownField::Invalid;
    }
}

TagDataType Id3v2Tag::internallyGetProposedDataType(const uint32 &id) const
{
    using namespace Id3v2FrameIds;
    switch (id) {
    case lLength:
    case sLength:
        return TagDataType::TimeSpan;
    case lBpm:
    case sBpm:
        return TagDataType::Integer;
    case lTrackPosition:
    case sTrackPosition:
    case lDiskPosition:
        return TagDataType::PositionInSet;
    case lCover:
    case sCover:
        return TagDataType::Picture;
    default:
        if (Id3v2FrameIds::isTextFrame(id)) {
            return TagDataType::Text;
        } else {
            return TagDataType::Undefined;
        }
    }
}

/*!
 * \brief Parses tag information from the specified \a stream.
 *
 * \throws Throws std::ios_base::failure when an IO error occurs.
 * \throws Throws TagParser::Failure or a derived exception when a parsing
 *         error occurs.
 */
void Id3v2Tag::parse(istream &stream, const uint64 maximalSize, Diagnostics &diag)
{
    // prepare parsing
    static const string context("parsing ID3v2 tag");
    BinaryReader reader(&stream);
    const auto startOffset = static_cast<uint64>(stream.tellg());

    // check whether the header is truncated
    if (maximalSize && maximalSize < 10) {
        diag.emplace_back(DiagLevel::Critical, "ID3v2 header is truncated (at least 10 bytes expected).", context);
        throw TruncatedDataException();
    }

    // read signature: ID3
    if (reader.readUInt24BE() != 0x494433u) {
        diag.emplace_back(DiagLevel::Critical, "Signature is invalid.", context);
        throw InvalidDataException();
    }
    // read header data
    byte majorVersion = reader.readByte();
    byte revisionVersion = reader.readByte();
    setVersion(majorVersion, revisionVersion);
    m_flags = reader.readByte();
    m_sizeExcludingHeader = reader.readSynchsafeUInt32BE();
    m_size = 10 + m_sizeExcludingHeader;
    if (m_sizeExcludingHeader == 0) {
        diag.emplace_back(DiagLevel::Warning, "ID3v2 tag seems to be empty.", context);
        return;
    }

    // check if the version
    if (!isVersionSupported()) {
        diag.emplace_back(DiagLevel::Critical, "The ID3v2 tag couldn't be parsed, because its version is not supported.", context);
        throw VersionNotSupportedException();
    }

    // read extended header (if present)
    if (hasExtendedHeader()) {
        if (maximalSize && maximalSize < 14) {
            diag.emplace_back(DiagLevel::Critical, "Extended header denoted but not present.", context);
            throw TruncatedDataException();
        }
        m_extendedHeaderSize = reader.readSynchsafeUInt32BE();
        if (m_extendedHeaderSize < 6 || m_extendedHeaderSize > m_sizeExcludingHeader || (maximalSize && maximalSize < (10 + m_extendedHeaderSize))) {
            diag.emplace_back(DiagLevel::Critical, "Extended header is invalid/truncated.", context);
            throw TruncatedDataException();
        }
        stream.seekg(m_extendedHeaderSize - 4, ios_base::cur);
    }

    // how many bytes remain for frames and padding?
    uint32 bytesRemaining = m_sizeExcludingHeader - m_extendedHeaderSize;
    if (maximalSize && bytesRemaining > maximalSize) {
        bytesRemaining = static_cast<uint32>(maximalSize);
        diag.emplace_back(DiagLevel::Critical, "Frames are truncated.", context);
    }

    // read frames
    auto pos = static_cast<uint64>(stream.tellg());
    while (bytesRemaining) {
        // seek to next frame
        stream.seekg(static_cast<streamoff>(pos));
        // parse frame
        Id3v2Frame frame;
        try {
            frame.parse(reader, majorVersion, bytesRemaining, diag);
            if (Id3v2FrameIds::isTextFrame(frame.id()) && fields().count(frame.id()) == 1) {
                diag.emplace_back(DiagLevel::Warning, "The text frame " % frame.frameIdString() + " exists more than once.", context);
            }
            fields().emplace(frame.id(), move(frame));
        } catch (const NoDataFoundException &) {
            if (frame.hasPaddingReached()) {
                m_paddingSize = startOffset + m_size - pos;
                break;
            }
        } catch (const Failure &) {
        }

        // calculate next frame offset
        if (frame.totalSize() <= bytesRemaining) {
            pos += frame.totalSize();
            bytesRemaining -= frame.totalSize();
        } else {
            pos += bytesRemaining;
            bytesRemaining = 0;
        }
    }

    // check for extended header
    if (!hasFooter()) {
        return;
    }
    if (maximalSize && m_size + 10 < maximalSize) {
        // the footer does not provide additional information, just check the signature
        stream.seekg(static_cast<streamoff>(startOffset + (m_size += 10)));
        if (reader.readUInt24LE() != 0x494433u) {
            diag.emplace_back(DiagLevel::Critical, "Footer signature is invalid.", context);
        }
        // skip remaining footer
        stream.seekg(7, ios_base::cur);
    } else {
        diag.emplace_back(DiagLevel::Critical, "Footer denoted but not present.", context);
        throw TruncatedDataException();
    }
}

/*!
 * \brief Prepares making.
 * \returns Returns a Id3v2TagMaker object which can be used to actually make the tag.
 * \remarks The tag must NOT be mutated after making is prepared when it is intended to actually
 *          make the tag using the make method of the returned object.
 * \throws Throws TagParser::Failure or a derived exception when a making error occurs.
 *
 * This method might be useful when it is necessary to know the size of the tag before making it.
 * \sa make()
 */
Id3v2TagMaker Id3v2Tag::prepareMaking(Diagnostics &diag)
{
    return Id3v2TagMaker(*this, diag);
}

/*!
 * \brief Writes tag information to the specified \a stream.
 *
 * \throws Throws std::ios_base::failure when an IO error occurs.
 * \throws Throws TagParser::Failure or a derived exception when a making
 *                error occurs.
 */
void Id3v2Tag::make(ostream &stream, uint32 padding, Diagnostics &diag)
{
    prepareMaking(diag).make(stream, padding, diag);
}

/*!
 * \brief Sets the version to the specified \a majorVersion and
 *        the specified \a revisionVersion.
 */
void Id3v2Tag::setVersion(byte majorVersion, byte revisionVersion)
{
    m_majorVersion = majorVersion;
    m_revisionVersion = revisionVersion;
    m_version = argsToString('2', '.', majorVersion, '.', revisionVersion);
}

/*!
 * \class TagParser::FrameComparer
 * \brief Defines the order which is used to store ID3v2 frames.
 *
 * The order is: unique file id, title, other text frames, other frames, cover
 */

/*!
 * \brief Returns true if \a lhs goes before \a rhs; otherwise returns false.
 * \todo Don't pass args by reference in v8.
 */
bool FrameComparer::operator()(const uint32 &lhsRef, const uint32 &rhsRef) const
{
    uint32 lhs(lhsRef);
    uint32 rhs(rhsRef);

    if (lhs == rhs) {
        return false;
    }

    const bool lhsLong = Id3v2FrameIds::isLongId(lhs);
    const bool rhsLong = Id3v2FrameIds::isLongId(rhs);
    if (lhsLong != rhsLong) {
        if (!lhsLong) {
            lhs = Id3v2FrameIds::convertToLongId(lhs);
        } else if (!rhsLong) {
            rhs = Id3v2FrameIds::convertToLongId(rhs);
        }
    }

    if (lhs == Id3v2FrameIds::lUniqueFileId || lhs == Id3v2FrameIds::sUniqueFileId) {
        return true;
    }
    if (rhs == Id3v2FrameIds::lUniqueFileId || rhs == Id3v2FrameIds::sUniqueFileId) {
        return false;
    }
    if (lhs == Id3v2FrameIds::lTitle || lhs == Id3v2FrameIds::sTitle) {
        return true;
    }
    if (rhs == Id3v2FrameIds::lTitle || rhs == Id3v2FrameIds::sTitle) {
        return false;
    }

    const bool lhstextfield = Id3v2FrameIds::isTextFrame(lhs);
    const bool rhstextfield = Id3v2FrameIds::isTextFrame(rhs);
    if (lhstextfield && !rhstextfield) {
        return true;
    }
    if (!lhstextfield && rhstextfield) {
        return false;
    }

    if (lhs == Id3v2FrameIds::lCover || lhs == Id3v2FrameIds::sCover) {
        return false;
    }
    if (rhs == Id3v2FrameIds::lCover || rhs == Id3v2FrameIds::sCover) {
        return true;
    }
    return lhs < rhs;
}

/*!
 * \class TagParser::Id3v2TagMaker
 * \brief The Id3v2TagMaker class helps writing ID3v2 tags.
 *
 * An instance can be obtained using the Id3v2Tag::prepareMaking() method.
 */

/*!
 * \brief Prepares making the specified \a tag.
 * \sa See Id3v2Tag::prepareMaking() for more information.
 */
Id3v2TagMaker::Id3v2TagMaker(Id3v2Tag &tag, Diagnostics &diag)
    : m_tag(tag)
    , m_framesSize(0)
{
    static const string context("making ID3v2 tag");

    // check if version is supported
    // (the version could have been changed using setVersion())
    if (!tag.isVersionSupported()) {
        diag.emplace_back(DiagLevel::Critical, "The ID3v2 tag version isn't supported.", context);
        throw VersionNotSupportedException();
    }

    // prepare frames
    m_maker.reserve(tag.fields().size());
    for (auto &pair : tag.fields()) {
        try {
            m_maker.emplace_back(pair.second.prepareMaking(tag.majorVersion(), diag));
            m_framesSize += m_maker.back().requiredSize();
        } catch (const Failure &) {
        }
    }

    // calculate required size
    // -> header + size of frames
    m_requiredSize = 10 + m_framesSize;
}

/*!
 * \brief Saves the tag (specified when constructing the object) to the
 *        specified \a stream.
 * \throws Throws std::ios_base::failure when an IO error occurs.
 * \throws Throws Assumes the data is already validated and thus does NOT
 *                throw TagParser::Failure or a derived exception.
 */
void Id3v2TagMaker::make(std::ostream &stream, uint32 padding, Diagnostics &diag)
{
    VAR_UNUSED(diag)

    BinaryWriter writer(&stream);

    // write header
    // -> signature
    writer.writeUInt24BE(0x494433u);
    // -> version
    writer.writeByte(m_tag.majorVersion());
    writer.writeByte(m_tag.revisionVersion());
    // -> flags, but without extended header or compression bit set
    writer.writeByte(m_tag.flags() & 0xBF);
    // -> size (excluding header)
    writer.writeSynchsafeUInt32BE(m_framesSize + padding);

    // write frames
    for (auto &maker : m_maker) {
        maker.make(writer);
    }

    // write padding
    for (; padding; --padding) {
        stream.put(0);
    }
}

} // namespace TagParser
