#include "./matroskacues.h"
#include "./matroskacontainer.h"

#include <c++utilities/conversion/binaryconversion.h>

using namespace std;
using namespace ConversionUtilities;

namespace Media {

/*!
 * \class Media::MatroskaCuePositionUpdater
 * \brief The MatroskaCuePositionUpdater class helps to rewrite the "Cues"-element with shifted positions.
 *
 * This class is used when rewriting Matroska files to save changed tag information.
 */

/*!
 * \brief Returns how many bytes will be written when calling the make() method.
 * \remarks The returned size might change when the object is altered (eg. by calling the updatePositions() method).
 */
uint64 MatroskaCuePositionUpdater::totalSize() const
{
    if(m_cuesElement) {
        uint64 size = m_sizes.at(m_cuesElement);
        return 4 + EbmlElement::calculateSizeDenotationLength(size) + size;
    } else {
        return 0;
    }
}

/*!
 * \brief Parses the specified \a cuesElement.
 * \remarks Previous parsing results and updates will be cleared.
 */
void MatroskaCuePositionUpdater::parse(EbmlElement *cuesElement)
{
    static const string context("parsing \"Cues\"-element");
    clear();
    uint64 cuesElementSize = 0, cuePointElementSize, cueTrackPositionsElementSize, cueReferenceElementSize, pos, relPos, statePos;
    EbmlElement *cueRelativePositionElement, *cueClusterPositionElement;
    for(EbmlElement *cuePointElement = cuesElement->firstChild(); cuePointElement; cuePointElement = cuePointElement->nextSibling()) {
        // parse childs of "Cues"-element which must be "CuePoint"-elements
        cuePointElement->parse();
        switch(cuePointElement->id()) {
        case EbmlIds::Void:
        case EbmlIds::Crc32:
            break;
        case MatroskaIds::CuePoint:
            cuePointElementSize = 0;
            for(EbmlElement *cuePointChild = cuePointElement->firstChild(); cuePointChild; cuePointChild = cuePointChild->nextSibling()) {
                // parse childs of "CuePoint"-element
                cuePointChild->parse();
                switch(cuePointChild->id()) {
                case EbmlIds::Void:
                case EbmlIds::Crc32:
                    break;
                case MatroskaIds::CueTime:
                    cuePointChild->makeBuffer();
                    cuePointElementSize += cuePointChild->totalSize();
                    break;
                case MatroskaIds::CueTrackPositions:
                    cueTrackPositionsElementSize = 0;
                    cueRelativePositionElement = cueClusterPositionElement = nullptr;
                    for(EbmlElement *cueTrackPositionsChild = cuePointChild->firstChild(); cueTrackPositionsChild; cueTrackPositionsChild = cueTrackPositionsChild->nextSibling()) {
                        // parse childs of "CueTrackPositions"-element
                        cueTrackPositionsChild->parse();                        
                        switch(cueTrackPositionsChild->id()) {
                        case MatroskaIds::CueTrack:
                        case MatroskaIds::CueDuration:
                        case MatroskaIds::CueBlockNumber:
                            cueTrackPositionsChild->makeBuffer();
                            cueTrackPositionsElementSize += cueTrackPositionsChild->totalSize();
                            break;
                        case MatroskaIds::CueRelativePosition:
                            relPos = (cueRelativePositionElement = cueTrackPositionsChild)->readUInteger();
                            break;
                        case MatroskaIds::CueClusterPosition:
                            pos = (cueClusterPositionElement = cueTrackPositionsChild)->readUInteger();
                            cueTrackPositionsElementSize += 2 + EbmlElement::calculateUIntegerLength(pos);
                            m_offsets.emplace(cueTrackPositionsChild, pos);
                            break;
                        case MatroskaIds::CueCodecState:
                            statePos = cueTrackPositionsChild->readUInteger();
                            cueTrackPositionsElementSize += 2 + EbmlElement::calculateUIntegerLength(statePos);
                            m_offsets.emplace(cueTrackPositionsChild, statePos);
                            break;
                        case MatroskaIds::CueReference:
                            cueReferenceElementSize = 0;
                            for(EbmlElement *cueReferenceChild = cueTrackPositionsChild->firstChild(); cueReferenceChild; cueReferenceChild = cueReferenceChild->nextSibling()) {
                                // parse childs of "CueReference"-element
                                cueReferenceChild->parse();
                                switch(cueReferenceChild->id()) {
                                case EbmlIds::Void:
                                case EbmlIds::Crc32:
                                    break;
                                case MatroskaIds::CueRefTime:
                                case MatroskaIds::CueRefNumber:
                                    cueReferenceChild->makeBuffer();
                                    cueReferenceElementSize += cueReferenceChild->totalSize();
                                    break;
                                case MatroskaIds::CueRefCluster:
                                case MatroskaIds::CueRefCodecState:
                                    statePos = cueReferenceChild->readUInteger();
                                    cueReferenceElementSize += 2 + EbmlElement::calculateUIntegerLength(statePos);
                                    m_offsets.emplace(cueReferenceChild, statePos);
                                    break;
                                default:
                                    addNotification(NotificationType::Warning, "\"CueReference\"-element contains a element which is not known to the parser. It will be ignored.", context);
                                }
                            }
                            cueTrackPositionsElementSize += 1 + EbmlElement::calculateSizeDenotationLength(cueReferenceElementSize) + cueReferenceElementSize;
                            m_sizes.emplace(cueTrackPositionsChild, cueReferenceElementSize);
                            break;
                        default:
                            addNotification(NotificationType::Warning, "\"CueTrackPositions\"-element contains a element which is not known to the parser. It will be ignored.", context);
                        }
                    }
                    if(!cueClusterPositionElement) {
                        addNotification(NotificationType::Critical, "\"CueTrackPositions\"-element does not contain mandatory \"CueClusterPosition\"-element.", context);
                    } else if(cueRelativePositionElement) {
                        cueTrackPositionsElementSize += 2 + EbmlElement::calculateUIntegerLength(relPos);
                        m_relativeOffsets.emplace(piecewise_construct, forward_as_tuple(cueRelativePositionElement), forward_as_tuple(pos, relPos));
                    }
                    cuePointElementSize += 1 + EbmlElement::calculateSizeDenotationLength(cueTrackPositionsElementSize) + cueTrackPositionsElementSize;
                    m_sizes.emplace(cuePointChild, cueTrackPositionsElementSize);
                    break;
                default:
                    addNotification(NotificationType::Warning, "\"CuePoint\"-element contains a element which is not a \"CueTime\"- or a \"CueTrackPositions\"-element. It will be ignored.", context);
                }
            }
            cuesElementSize += 1 + EbmlElement::calculateSizeDenotationLength(cuePointElementSize) + cuePointElementSize;
            m_sizes.emplace(cuePointElement, cuePointElementSize);
            break;
        default:
            addNotification(NotificationType::Warning, "\"Cues\"-element contains a element which is not a \"CuePoint\"-element. It will be ignored.", context);
        }
    }
    m_sizes.emplace(m_cuesElement = cuesElement, cuesElementSize);
}

/*!
 * \brief Sets the offset of the entries with the specified \a originalOffset to \a newOffset.
 * \returns Returns whether the size of the "Cues"-element has been altered.
 */
bool MatroskaCuePositionUpdater::updateOffsets(uint64 originalOffset, uint64 newOffset)
{
    bool updated = false;
    for(auto &offset : m_offsets) {
        if(offset.second.initialValue() == originalOffset && offset.second.currentValue() != newOffset) {
            updated = updateSize(offset.first->parent(), static_cast<int>(EbmlElement::calculateUIntegerLength(newOffset)) - static_cast<int>(EbmlElement::calculateUIntegerLength(offset.second.currentValue()))) || updated;
            offset.second.update(newOffset);
        }
    }
    return updated;
}

/*!
 * \brief Sets the relative offset of the entries with the specified \a originalRelativeOffset and the specified \a referenceOffset to \a newRelativeOffset.
 * \returns Returns whether the size of the "Cues"-element has been altered.
 */
bool MatroskaCuePositionUpdater::updateRelativeOffsets(uint64 referenceOffset, uint64 originalRelativeOffset, uint64 newRelativeOffset)
{
    bool updated = false;
    for(auto &offset : m_relativeOffsets) {
        if(offset.second.referenceOffset() == referenceOffset && offset.second.initialValue() == originalRelativeOffset && offset.second.currentValue() != newRelativeOffset) {
            updated = updateSize(offset.first->parent(), static_cast<int>(EbmlElement::calculateUIntegerLength(newRelativeOffset)) - static_cast<int>(EbmlElement::calculateUIntegerLength(offset.second.currentValue()))) || updated;
            offset.second.update(newRelativeOffset);
        }
    }
    return updated;
}

/*!
 * \brief Updates the sizes for the specified \a element by adding the specified \a shift value.
 * \returns Returns whether the size of the "Cues"-element has been altered.
 */
bool MatroskaCuePositionUpdater::updateSize(EbmlElement *element, int shift)
{
    if(!shift) {
        // shift is gone
        return false;
    }
    if(!element) {
        // there was no parent (shouldn't happen in a normal file structure since the Segment element should
        // be parent of the Cues element)
        return shift;
    }
    try {
        // get size info
        uint64 &size = m_sizes.at(element);
        // calculate new size
        uint64 newSize = shift > 0 ? size + static_cast<uint64>(shift) : size - static_cast<uint64>(-shift);
        // shift parent
        bool updated = updateSize(element->parent(), shift + static_cast<int>(EbmlElement::calculateSizeDenotationLength(newSize)) - static_cast<int>(EbmlElement::calculateSizeDenotationLength(size)));
        // apply new size
        size = newSize;
        return updated;
    } catch(const out_of_range &) {
        // the element is out of the scope of the cue position updater (likely the Segment element)
        return shift;
    }
}

/*!
 * \brief Writes the previously parsed "Cues"-element with updates positions to the specified \a stream.
 */
void MatroskaCuePositionUpdater::make(ostream &stream)
{
    static const string context("making \"Cues\"-element");
    if(!m_cuesElement) {
        addNotification(NotificationType::Warning, "No cues written; the cues of the source file could not be parsed correctly.", context);
        return;
    }
    // temporary variables
    char buff[8];
    byte len;
    // write "Cues"-element
    try {
        BE::getBytes(static_cast<uint32>(MatroskaIds::Cues), buff);
        stream.write(buff, 4);
        len = EbmlElement::makeSizeDenotation(m_sizes[m_cuesElement], buff);
        stream.write(buff, len);
        // loop through original elements and write (a updated version) of them
        for(EbmlElement *cuePointElement = m_cuesElement->firstChild(); cuePointElement; cuePointElement = cuePointElement->nextSibling()) {
            cuePointElement->parse();
            switch(cuePointElement->id()) {
            case EbmlIds::Void:
            case EbmlIds::Crc32:
                break;
            case MatroskaIds::CuePoint:
                // write "CuePoint"-element
                stream.put(MatroskaIds::CuePoint);
                len = EbmlElement::makeSizeDenotation(m_sizes[cuePointElement], buff);
                stream.write(buff, len);
                for(EbmlElement *cuePointChild = cuePointElement->firstChild(); cuePointChild; cuePointChild = cuePointChild->nextSibling()) {
                    cuePointChild->parse();
                    switch(cuePointChild->id()) {
                    case EbmlIds::Void:
                    case EbmlIds::Crc32:
                        break;
                    case MatroskaIds::CueTime:
                        // write "CueTime"-element
                        cuePointChild->copyBuffer(stream);
                        cuePointChild->discardBuffer();
                        //cuePointChild->copyEntirely(stream);
                        break;
                    case MatroskaIds::CueTrackPositions:
                        // write "CueTrackPositions"-element
                        stream.put(MatroskaIds::CueTrackPositions);
                        len = EbmlElement::makeSizeDenotation(m_sizes[cuePointChild], buff);
                        stream.write(buff, len);
                        for(EbmlElement *cueTrackPositionsChild = cuePointChild->firstChild(); cueTrackPositionsChild; cueTrackPositionsChild = cueTrackPositionsChild->nextSibling()) {
                            cueTrackPositionsChild->parse();
                            switch(cueTrackPositionsChild->id()) {
                            case MatroskaIds::CueTrack:
                            case MatroskaIds::CueDuration:
                            case MatroskaIds::CueBlockNumber:
                                // write unchanged childs of "CueTrackPositions"-element
                                cueTrackPositionsChild->copyBuffer(stream);
                                cueTrackPositionsChild->discardBuffer();
                                //cueTrackPositionsChild->copyEntirely(stream);
                                break;
                            case MatroskaIds::CueRelativePosition:
                                try {
                                    EbmlElement::makeSimpleElement(stream, cueTrackPositionsChild->id(), m_relativeOffsets.at(cueTrackPositionsChild).currentValue());
                                } catch(const out_of_range &) {
                                    // we were not able parse the relative offset because the absolute offset is missing
                                    // continue anyways
                                }
                                break;
                            case MatroskaIds::CueClusterPosition:
                            case MatroskaIds::CueCodecState:
                                // write "CueClusterPosition"/"CueCodecState"-element
                                EbmlElement::makeSimpleElement(stream, cueTrackPositionsChild->id(), m_offsets.at(cueTrackPositionsChild).currentValue());
                                break;
                            case MatroskaIds::CueReference:
                                // write "CueReference"-element
                                stream.put(MatroskaIds::CueRefTime);
                                len = EbmlElement::makeSizeDenotation(m_sizes[cueTrackPositionsChild], buff);
                                stream.write(buff, len);
                                for(EbmlElement *cueReferenceChild = cueTrackPositionsChild->firstChild(); cueReferenceChild; cueReferenceChild = cueReferenceChild->nextSibling()) {
                                    cueReferenceChild->parse();
                                    switch(cueReferenceChild->id()) {
                                    case EbmlIds::Void:
                                    case EbmlIds::Crc32:
                                        break;
                                    case MatroskaIds::CueRefTime:
                                    case MatroskaIds::CueRefNumber:
                                        // write unchanged childs of "CueReference"-element
                                        cueReferenceChild->copyBuffer(stream);
                                        cueReferenceChild->discardBuffer();
                                        cueReferenceChild->copyEntirely(stream);
                                        break;
                                    case MatroskaIds::CueRefCluster:
                                    case MatroskaIds::CueRefCodecState:
                                        // write "CueRefCluster"/"CueRefCodecState"-element
                                        EbmlElement::makeSimpleElement(stream, cueReferenceChild->id(), m_offsets.at(cueReferenceChild).currentValue());
                                        break;
                                    default:
                                        addNotification(NotificationType::Warning, "\"CueReference\"-element contains a element which is not known to the parser. It will be ignored.", context);
                                    }
                                }
                                break;
                            default:
                                addNotification(NotificationType::Warning, "\"CueTrackPositions\"-element contains a element which is not known to the parser. It will be ignored.", context);
                            }
                        }
                        break;
                    default:
                        addNotification(NotificationType::Warning, "\"CuePoint\"-element contains a element which is not a \"CueTime\"- or a \"CueTrackPositions\"-element. It will be ignored.", context);
                    }
                }
                break;
            default:
                addNotification(NotificationType::Warning, "\"Cues\"-element contains a element which is not a \"CuePoint\"-element. It will be ignored.", context);
            }
        }
    } catch(const out_of_range &) {
        addNotification(NotificationType::Critical, "Unable to write the file index because the index of the original file could not be parsed correctly.", context);
        throw InvalidDataException();
    }
}

} // namespace Media

