# libmphoto: A motion photo demuxer and remuxer
## Overview

This library provides an implementation for demuxing and remuxing motion photos (and microvideos) produced primarily by Android cameras. This library abstracts the reading/writing of XMP metadata which is used to parse and create these motion photos. 

## Terminology

- **Motion Photo** - Describes a Google authored specification for a file that contains both a still and a video. The still must be a JPEG or HEIC, and the video must be an MP4.
- **Microvideo**  - Describes a now deprecated specification that is replaced by motion photos.
- **Demuxing** - Splitting a multi-part stream into individual parts (ie. splitting a motion photo into a still and video).
- **Remuxing** - Combining multiple streams into a single stream (ie. combining a still and a video into a motion photo).

## Usage

### Demuxer

The demuxer enables the extraction of information and encoded media streams from a motion photo.

#### Support
| Type               | Supported |
|--------------------|-----------|
| JPEG Motion Photo | ✅         |
| HEIC Motion Photo | ✅         |
| Microvideo        | ✅         |

#### Example
```
// Initalize the demuxer
Demuxer demuxer;
demuxer.Init(motion_photo);

// Extract the information
ImageInfo image_info;
demuxer.GetInfo(&image_info);

// Extract the still stream
string still;
demuxer.GetStill(&still);

// Extract the video stream
string video;
demuxer.GetVideo(&video)
```
*see samples/demux.cc for complete example code*

### Remuxer

The remuxer enables combining an encoded still and video stream to produce a motion photo. If the still has existing XMP metadata, it will persist into the new motion photo. If the still has existing microvideo XMP metadata, the result will be a microvideo. In all other cases, the resultant stream is a motion photo.

#### Support
| Type                   | Supported |
|------------------------|-----------|
| JPEG Still + Mp4 Video | ✅        |
| HEIC Still + Mp4 Video | ❌        |

*Due to a limitation of libheif, XMP cannot be added without decoding and re-encoding the HEIC. As such, remuxing with a HEIC still is not currently supported. Related libheif ticket: [libheif/issues/127](https://github.com/strukturag/libheif/issues/127).*


#### Example
```
// Initalize the remuxer
Remuxer remuxer;

// Set the media streams
remuxer.SetStill(still);
remuxer.SetVideo(video);

// Generate the motion photo
string motion_photo;
remuxer.Finalize(&motion_photo);
```
*see samples/remux.cc for complete example code*

## Testing
This library has a set of unit tests that verify demuxing and remuxing functionality against a set of golden images. These tests depend on [googletest](http://github.com/google/googletest) and can be run with bazel using `bazel test //tests/...`.

## Fuzzing
This library is setup to be fuzz tested with [libfuzzer](https://llvm.org/docs/LibFuzzer.html) through [bazel-rules-fuzzing](https://github.com/googleinterns/bazel-rules-fuzzing). Fuzzing can be run using `bazel test --config=(asan|msan)-libfuzzer //fuzz:(demuxer|remuxer)_fuzz_test`
