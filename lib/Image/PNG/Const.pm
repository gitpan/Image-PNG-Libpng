package Image::PNG::Const;
our $VERSION = '0.31';

require Exporter;
@ISA = qw(Exporter);
@EXPORT_OK = qw/
PNG_DISPOSE_OP_NONE
PNG_DISPOSE_OP_BACKGROUND
PNG_DISPOSE_OP_PREVIOUS
PNG_BLEND_OP_SOURCE
PNG_BLEND_OP_OVER
PNG_TEXT_COMPRESSION_NONE_WR
PNG_TEXT_COMPRESSION_zTXt_WR
PNG_TEXT_COMPRESSION_NONE
PNG_TEXT_COMPRESSION_zTXt
PNG_ITXT_COMPRESSION_NONE
PNG_ITXT_COMPRESSION_zTXt
PNG_HAVE_IHDR
PNG_HAVE_PLTE
PNG_AFTER_IDAT
PNG_FP_1
PNG_FP_HALF
PNG_FP_MIN
PNG_COLOR_MASK_PALETTE
PNG_COLOR_MASK_COLOR
PNG_COLOR_MASK_ALPHA
PNG_COLOR_TYPE_GRAY
PNG_COLOR_TYPE_PALETTE
PNG_COLOR_TYPE_RGB
PNG_COLOR_TYPE_RGB_ALPHA
PNG_COLOR_TYPE_GRAY_ALPHA
PNG_COLOR_TYPE_RGBA
PNG_COLOR_TYPE_GA
PNG_COMPRESSION_TYPE_BASE
PNG_COMPRESSION_TYPE_DEFAULT
PNG_FILTER_TYPE_BASE
PNG_INTRAPIXEL_DIFFERENCING
PNG_FILTER_TYPE_DEFAULT
PNG_INTERLACE_NONE
PNG_INTERLACE_ADAM7
PNG_OFFSET_PIXEL
PNG_OFFSET_MICROMETER
PNG_EQUATION_LINEAR
PNG_EQUATION_BASE_E
PNG_EQUATION_ARBITRARY
PNG_EQUATION_HYPERBOLIC
PNG_SCALE_UNKNOWN
PNG_SCALE_METER
PNG_SCALE_RADIAN
PNG_RESOLUTION_UNKNOWN
PNG_RESOLUTION_METER
PNG_sRGB_INTENT_PERCEPTUAL
PNG_sRGB_INTENT_RELATIVE
PNG_sRGB_INTENT_SATURATION
PNG_sRGB_INTENT_ABSOLUTE
PNG_KEYWORD_MAX_LENGTH
PNG_MAX_PALETTE_LENGTH
PNG_INFO_gAMA
PNG_INFO_sBIT
PNG_INFO_cHRM
PNG_INFO_PLTE
PNG_INFO_tRNS
PNG_INFO_bKGD
PNG_INFO_hIST
PNG_INFO_pHYs
PNG_INFO_oFFs
PNG_INFO_tIME
PNG_INFO_pCAL
PNG_INFO_sRGB
PNG_INFO_iCCP
PNG_INFO_sPLT
PNG_INFO_sCAL
PNG_INFO_IDAT
PNG_INFO_acTL
PNG_INFO_fcTL
PNG_TRANSFORM_IDENTITY
PNG_TRANSFORM_STRIP_16
PNG_TRANSFORM_STRIP_ALPHA
PNG_TRANSFORM_PACKING
PNG_TRANSFORM_PACKSWAP
PNG_TRANSFORM_EXPAND
PNG_TRANSFORM_INVERT_MONO
PNG_TRANSFORM_SHIFT
PNG_TRANSFORM_BGR
PNG_TRANSFORM_SWAP_ALPHA
PNG_TRANSFORM_SWAP_ENDIAN
PNG_TRANSFORM_INVERT_ALPHA
PNG_TRANSFORM_STRIP_FILLER
PNG_TRANSFORM_STRIP_FILLER_BEFORE
PNG_TRANSFORM_STRIP_FILLER_AFTER
PNG_TRANSFORM_GRAY_TO_RGB
PNG_TRANSFORM_EXPAND_16
PNG_TRANSFORM_SCALE_16
PNG_FLAG_MNG_EMPTY_PLTE
PNG_FLAG_MNG_FILTER_64
PNG_ALL_MNG_FEATURES
PNG_ERROR_ACTION_NONE
PNG_ERROR_ACTION_WARN
PNG_ERROR_ACTION_ERROR
PNG_RGB_TO_GRAY_DEFAULT
PNG_ALPHA_PNG
PNG_ALPHA_STANDARD
PNG_ALPHA_ASSOCIATED
PNG_ALPHA_PREMULTIPLIED
PNG_ALPHA_OPTIMIZED
PNG_ALPHA_BROKEN
PNG_DEFAULT_sRGB
PNG_GAMMA_MAC_18
PNG_GAMMA_sRGB
PNG_GAMMA_LINEAR
PNG_FILLER_BEFORE
PNG_FILLER_AFTER
PNG_BACKGROUND_GAMMA_UNKNOWN
PNG_BACKGROUND_GAMMA_SCREEN
PNG_BACKGROUND_GAMMA_FILE
PNG_BACKGROUND_GAMMA_UNIQUE
PNG_READ_16_TO_8
PNG_GAMMA_THRESHOLD
PNG_CRC_DEFAULT
PNG_CRC_ERROR_QUIT
PNG_CRC_WARN_DISCARD
PNG_CRC_WARN_USE
PNG_CRC_QUIET_USE
PNG_CRC_NO_CHANGE
PNG_NO_FILTERS
PNG_FILTER_NONE
PNG_FILTER_SUB
PNG_FILTER_UP
PNG_FILTER_AVG
PNG_FILTER_PAETH
PNG_ALL_FILTERS
PNG_FILTER_VALUE_NONE
PNG_FILTER_VALUE_SUB
PNG_FILTER_VALUE_UP
PNG_FILTER_VALUE_AVG
PNG_FILTER_VALUE_PAETH
PNG_FILTER_HEURISTIC_DEFAULT
PNG_FILTER_HEURISTIC_UNWEIGHTED
PNG_FILTER_HEURISTIC_WEIGHTED
PNG_DESTROY_WILL_FREE_DATA
PNG_SET_WILL_FREE_DATA
PNG_USER_WILL_FREE_DATA
PNG_FREE_HIST
PNG_FREE_ICCP
PNG_FREE_SPLT
PNG_FREE_ROWS
PNG_FREE_PCAL
PNG_FREE_SCAL
PNG_FREE_UNKN
PNG_FREE_LIST
PNG_FREE_PLTE
PNG_FREE_TRNS
PNG_FREE_TEXT
PNG_FREE_ALL
PNG_FREE_MUL
PNG_HANDLE_CHUNK_AS_DEFAULT
PNG_HANDLE_CHUNK_NEVER
PNG_HANDLE_CHUNK_IF_SAFE
PNG_HANDLE_CHUNK_ALWAYS
PNG_IO_NONE
PNG_IO_READING
PNG_IO_WRITING
PNG_IO_SIGNATURE
PNG_IO_CHUNK_HDR
PNG_IO_CHUNK_DATA
PNG_IO_CHUNK_CRC
PNG_IO_MASK_OP
PNG_IO_MASK_LOC
PNG_INTERLACE_ADAM7_PASSES
/;

%EXPORT_TAGS = ('all' => \@EXPORT_OK);

use warnings;
use strict;

use constant {
    SUPPORTED => 1,
    PNG_GAMMA_THRESHOLD_FIXED => 1,
};

use constant {
    PNG_DISPOSE_OP_NONE => 0x00,
    PNG_DISPOSE_OP_BACKGROUND => 0x01,
    PNG_DISPOSE_OP_PREVIOUS => 0x02,
    PNG_BLEND_OP_SOURCE => 0x00,
    PNG_BLEND_OP_OVER => 0x01,
    PNG_TEXT_COMPRESSION_NONE_WR => -3,
    PNG_TEXT_COMPRESSION_zTXt_WR => -2,
    PNG_TEXT_COMPRESSION_NONE => -1,
    PNG_TEXT_COMPRESSION_zTXt => 0,
    PNG_ITXT_COMPRESSION_NONE => 1,
    PNG_ITXT_COMPRESSION_zTXt => 2,
    PNG_HAVE_IHDR => 0x01,
    PNG_HAVE_PLTE => 0x02,
    PNG_AFTER_IDAT => 0x08,
    PNG_FP_1 => 100000,
    PNG_FP_HALF => 50000,
    PNG_FP_MIN => (-PNG_FP_MAX),
    PNG_COLOR_MASK_PALETTE => 1,
    PNG_COLOR_MASK_COLOR => 2,
    PNG_COLOR_MASK_ALPHA => 4,
    PNG_COLOR_TYPE_GRAY => 0,
    PNG_COLOR_TYPE_PALETTE => (2 | 1),
    PNG_COLOR_TYPE_RGB => (2),
    PNG_COLOR_TYPE_RGB_ALPHA => (2 | 4),
    PNG_COLOR_TYPE_GRAY_ALPHA => (4),
    PNG_COLOR_TYPE_RGBA => (2 | 4),
    PNG_COLOR_TYPE_GA => (4),
    PNG_COMPRESSION_TYPE_BASE => 0,
    PNG_COMPRESSION_TYPE_DEFAULT => 0,
    PNG_FILTER_TYPE_BASE => 0,
    PNG_INTRAPIXEL_DIFFERENCING => 64,
    PNG_FILTER_TYPE_DEFAULT => 0,
    PNG_INTERLACE_NONE => 0,
    PNG_INTERLACE_ADAM7 => 1,
    PNG_OFFSET_PIXEL => 0,
    PNG_OFFSET_MICROMETER => 1,
    PNG_EQUATION_LINEAR => 0,
    PNG_EQUATION_BASE_E => 1,
    PNG_EQUATION_ARBITRARY => 2,
    PNG_EQUATION_HYPERBOLIC => 3,
    PNG_SCALE_UNKNOWN => 0,
    PNG_SCALE_METER => 1,
    PNG_SCALE_RADIAN => 2,
    PNG_RESOLUTION_UNKNOWN => 0,
    PNG_RESOLUTION_METER => 1,
    PNG_sRGB_INTENT_PERCEPTUAL => 0,
    PNG_sRGB_INTENT_RELATIVE => 1,
    PNG_sRGB_INTENT_SATURATION => 2,
    PNG_sRGB_INTENT_ABSOLUTE => 3,
    PNG_KEYWORD_MAX_LENGTH => 79,
    PNG_MAX_PALETTE_LENGTH => 256,
    PNG_INFO_gAMA => 0x0001,
    PNG_INFO_sBIT => 0x0002,
    PNG_INFO_cHRM => 0x0004,
    PNG_INFO_PLTE => 0x0008,
    PNG_INFO_tRNS => 0x0010,
    PNG_INFO_bKGD => 0x0020,
    PNG_INFO_hIST => 0x0040,
    PNG_INFO_pHYs => 0x0080,
    PNG_INFO_oFFs => 0x0100,
    PNG_INFO_tIME => 0x0200,
    PNG_INFO_pCAL => 0x0400,
    PNG_INFO_sRGB => 0x0800,
    PNG_INFO_iCCP => 0x1000,
    PNG_INFO_sPLT => 0x2000,
    PNG_INFO_sCAL => 0x4000,
    PNG_INFO_IDAT => 0x8000,
    PNG_INFO_acTL => 0x10000,
    PNG_INFO_fcTL => 0x20000,
    PNG_TRANSFORM_IDENTITY => 0x0000,
    PNG_TRANSFORM_STRIP_16 => 0x0001,
    PNG_TRANSFORM_STRIP_ALPHA => 0x0002,
    PNG_TRANSFORM_PACKING => 0x0004,
    PNG_TRANSFORM_PACKSWAP => 0x0008,
    PNG_TRANSFORM_EXPAND => 0x0010,
    PNG_TRANSFORM_INVERT_MONO => 0x0020,
    PNG_TRANSFORM_SHIFT => 0x0040,
    PNG_TRANSFORM_BGR => 0x0080,
    PNG_TRANSFORM_SWAP_ALPHA => 0x0100,
    PNG_TRANSFORM_SWAP_ENDIAN => 0x0200,
    PNG_TRANSFORM_INVERT_ALPHA => 0x0400,
    PNG_TRANSFORM_STRIP_FILLER => 0x0800,
    PNG_TRANSFORM_STRIP_FILLER_BEFORE => 0x0800,
    PNG_TRANSFORM_STRIP_FILLER_AFTER => 0x1000,
    PNG_TRANSFORM_GRAY_TO_RGB => 0x2000,
    PNG_TRANSFORM_EXPAND_16 => 0x4000,
    PNG_TRANSFORM_SCALE_16 => 0x8000,
    PNG_FLAG_MNG_EMPTY_PLTE => 0x01,
    PNG_FLAG_MNG_FILTER_64 => 0x04,
    PNG_ALL_MNG_FEATURES => 0x05,
    PNG_ERROR_ACTION_NONE => 1,
    PNG_ERROR_ACTION_WARN => 2,
    PNG_ERROR_ACTION_ERROR => 3,
    PNG_RGB_TO_GRAY_DEFAULT => (-1),
    PNG_ALPHA_PNG => 0,
    PNG_ALPHA_STANDARD => 1,
    PNG_ALPHA_ASSOCIATED => 1,
    PNG_ALPHA_PREMULTIPLIED => 1,
    PNG_ALPHA_OPTIMIZED => 2,
    PNG_ALPHA_BROKEN => 3,
    PNG_DEFAULT_sRGB => -1,
    PNG_GAMMA_MAC_18 => -2,
    PNG_GAMMA_sRGB => 220000,
    PNG_GAMMA_LINEAR => 100000,
    PNG_FILLER_BEFORE => 0,
    PNG_FILLER_AFTER => 1,
    PNG_BACKGROUND_GAMMA_UNKNOWN => 0,
    PNG_BACKGROUND_GAMMA_SCREEN => 1,
    PNG_BACKGROUND_GAMMA_FILE => 2,
    PNG_BACKGROUND_GAMMA_UNIQUE => 3,
    PNG_READ_16_TO_8 => SUPPORTED,
    PNG_GAMMA_THRESHOLD => (PNG_GAMMA_THRESHOLD_FIXED*.00001),
    PNG_CRC_DEFAULT => 0,
    PNG_CRC_ERROR_QUIT => 1,
    PNG_CRC_WARN_DISCARD => 2,
    PNG_CRC_WARN_USE => 3,
    PNG_CRC_QUIET_USE => 4,
    PNG_CRC_NO_CHANGE => 5,
    PNG_NO_FILTERS => 0x00,
    PNG_FILTER_NONE => 0x08,
    PNG_FILTER_SUB => 0x10,
    PNG_FILTER_UP => 0x20,
    PNG_FILTER_AVG => 0x40,
    PNG_FILTER_PAETH => 0x80,
    PNG_ALL_FILTERS => (0x08 | 0x10 | 0x20 | 0x40 | 0x80),
    PNG_FILTER_VALUE_NONE => 0,
    PNG_FILTER_VALUE_SUB => 1,
    PNG_FILTER_VALUE_UP => 2,
    PNG_FILTER_VALUE_AVG => 3,
    PNG_FILTER_VALUE_PAETH => 4,
    PNG_FILTER_HEURISTIC_DEFAULT => 0,
    PNG_FILTER_HEURISTIC_UNWEIGHTED => 1,
    PNG_FILTER_HEURISTIC_WEIGHTED => 2,
    PNG_DESTROY_WILL_FREE_DATA => 1,
    PNG_SET_WILL_FREE_DATA => 1,
    PNG_USER_WILL_FREE_DATA => 2,
    PNG_FREE_HIST => 0x0008,
    PNG_FREE_ICCP => 0x0010,
    PNG_FREE_SPLT => 0x0020,
    PNG_FREE_ROWS => 0x0040,
    PNG_FREE_PCAL => 0x0080,
    PNG_FREE_SCAL => 0x0100,
    PNG_FREE_UNKN => 0x0200,
    PNG_FREE_LIST => 0x0400,
    PNG_FREE_PLTE => 0x1000,
    PNG_FREE_TRNS => 0x2000,
    PNG_FREE_TEXT => 0x4000,
    PNG_FREE_ALL => 0x7fff,
    PNG_FREE_MUL => 0x4220,
    PNG_HANDLE_CHUNK_AS_DEFAULT => 0,
    PNG_HANDLE_CHUNK_NEVER => 1,
    PNG_HANDLE_CHUNK_IF_SAFE => 2,
    PNG_HANDLE_CHUNK_ALWAYS => 3,
    PNG_IO_NONE => 0x0000,
    PNG_IO_READING => 0x0001,
    PNG_IO_WRITING => 0x0002,
    PNG_IO_SIGNATURE => 0x0010,
    PNG_IO_CHUNK_HDR => 0x0020,
    PNG_IO_CHUNK_DATA => 0x0040,
    PNG_IO_CHUNK_CRC => 0x0080,
    PNG_IO_MASK_OP => 0x000f,
    PNG_IO_MASK_LOC => 0x00f0,
    PNG_INTERLACE_ADAM7_PASSES => 7,
};

=head1 NAME

Image::PNG::Const - make libpng constants available

=head1 SYNOPSIS

    # Get just the constants you need:
    use Image::PNG::Const qw/PNG_TRANSFORM_INVERT_ALPHA
                             PNG_COLOR_MASK_ALPHA/;
    # Get everything:
    use Image::PNG::Const ':all';

This module is a helper for L<Image::PNG::Libpng>. It contains no
functions, only constants.

=head1 CONSTANTS

This file contains the following constants, from the header
file of the PNG library, C<png.h>:

=over

=item PNG_DISPOSE_OP_NONE

PNG_DISPOSE_OP_NONE has value 0x00.

=item PNG_DISPOSE_OP_BACKGROUND

PNG_DISPOSE_OP_BACKGROUND has value 0x01.

=item PNG_DISPOSE_OP_PREVIOUS

PNG_DISPOSE_OP_PREVIOUS has value 0x02.

=item PNG_BLEND_OP_SOURCE

PNG_BLEND_OP_SOURCE has value 0x00.

=item PNG_BLEND_OP_OVER

PNG_BLEND_OP_OVER has value 0x01.

=item PNG_TEXT_COMPRESSION_NONE_WR

PNG_TEXT_COMPRESSION_NONE_WR has value -3.

=item PNG_TEXT_COMPRESSION_zTXt_WR

PNG_TEXT_COMPRESSION_zTXt_WR has value -2.

=item PNG_TEXT_COMPRESSION_NONE

PNG_TEXT_COMPRESSION_NONE has value -1.

=item PNG_TEXT_COMPRESSION_zTXt

PNG_TEXT_COMPRESSION_zTXt has value 0.

=item PNG_ITXT_COMPRESSION_NONE

PNG_ITXT_COMPRESSION_NONE has value 1.

=item PNG_ITXT_COMPRESSION_zTXt

PNG_ITXT_COMPRESSION_zTXt has value 2.

=item PNG_HAVE_IHDR

PNG_HAVE_IHDR has value 0x01.

=item PNG_HAVE_PLTE

PNG_HAVE_PLTE has value 0x02.

=item PNG_AFTER_IDAT

PNG_AFTER_IDAT has value 0x08.

=item PNG_FP_1

PNG_FP_1 has value 100000.

=item PNG_FP_HALF

PNG_FP_HALF has value 50000.

=item PNG_FP_MIN

PNG_FP_MIN has value (-PNG_FP_MAX).

=item PNG_COLOR_MASK_PALETTE

PNG_COLOR_MASK_PALETTE has value 1.

=item PNG_COLOR_MASK_COLOR

PNG_COLOR_MASK_COLOR has value 2.

=item PNG_COLOR_MASK_ALPHA

PNG_COLOR_MASK_ALPHA has value 4.

=item PNG_COLOR_TYPE_GRAY

PNG_COLOR_TYPE_GRAY has value 0.

=item PNG_COLOR_TYPE_PALETTE

PNG_COLOR_TYPE_PALETTE has value (2 | 1).

=item PNG_COLOR_TYPE_RGB

PNG_COLOR_TYPE_RGB has value (2).

=item PNG_COLOR_TYPE_RGB_ALPHA

PNG_COLOR_TYPE_RGB_ALPHA has value (2 | 4).

=item PNG_COLOR_TYPE_GRAY_ALPHA

PNG_COLOR_TYPE_GRAY_ALPHA has value (4).

=item PNG_COLOR_TYPE_RGBA

PNG_COLOR_TYPE_RGBA has value (2 | 4).

=item PNG_COLOR_TYPE_GA

PNG_COLOR_TYPE_GA has value (4).

=item PNG_COMPRESSION_TYPE_BASE

PNG_COMPRESSION_TYPE_BASE has value 0.

=item PNG_COMPRESSION_TYPE_DEFAULT

PNG_COMPRESSION_TYPE_DEFAULT has value 0.

=item PNG_FILTER_TYPE_BASE

PNG_FILTER_TYPE_BASE has value 0.

=item PNG_INTRAPIXEL_DIFFERENCING

PNG_INTRAPIXEL_DIFFERENCING has value 64.

=item PNG_FILTER_TYPE_DEFAULT

PNG_FILTER_TYPE_DEFAULT has value 0.

=item PNG_INTERLACE_NONE

PNG_INTERLACE_NONE has value 0.

=item PNG_INTERLACE_ADAM7

PNG_INTERLACE_ADAM7 has value 1.

=item PNG_OFFSET_PIXEL

PNG_OFFSET_PIXEL has value 0.

=item PNG_OFFSET_MICROMETER

PNG_OFFSET_MICROMETER has value 1.

=item PNG_EQUATION_LINEAR

PNG_EQUATION_LINEAR has value 0.

=item PNG_EQUATION_BASE_E

PNG_EQUATION_BASE_E has value 1.

=item PNG_EQUATION_ARBITRARY

PNG_EQUATION_ARBITRARY has value 2.

=item PNG_EQUATION_HYPERBOLIC

PNG_EQUATION_HYPERBOLIC has value 3.

=item PNG_SCALE_UNKNOWN

PNG_SCALE_UNKNOWN has value 0.

=item PNG_SCALE_METER

PNG_SCALE_METER has value 1.

=item PNG_SCALE_RADIAN

PNG_SCALE_RADIAN has value 2.

=item PNG_RESOLUTION_UNKNOWN

PNG_RESOLUTION_UNKNOWN has value 0.

=item PNG_RESOLUTION_METER

PNG_RESOLUTION_METER has value 1.

=item PNG_sRGB_INTENT_PERCEPTUAL

PNG_sRGB_INTENT_PERCEPTUAL has value 0.

=item PNG_sRGB_INTENT_RELATIVE

PNG_sRGB_INTENT_RELATIVE has value 1.

=item PNG_sRGB_INTENT_SATURATION

PNG_sRGB_INTENT_SATURATION has value 2.

=item PNG_sRGB_INTENT_ABSOLUTE

PNG_sRGB_INTENT_ABSOLUTE has value 3.

=item PNG_KEYWORD_MAX_LENGTH

PNG_KEYWORD_MAX_LENGTH has value 79.

=item PNG_MAX_PALETTE_LENGTH

PNG_MAX_PALETTE_LENGTH has value 256.

=item PNG_INFO_gAMA

PNG_INFO_gAMA has value 0x0001.

=item PNG_INFO_sBIT

PNG_INFO_sBIT has value 0x0002.

=item PNG_INFO_cHRM

PNG_INFO_cHRM has value 0x0004.

=item PNG_INFO_PLTE

PNG_INFO_PLTE has value 0x0008.

=item PNG_INFO_tRNS

PNG_INFO_tRNS has value 0x0010.

=item PNG_INFO_bKGD

PNG_INFO_bKGD has value 0x0020.

=item PNG_INFO_hIST

PNG_INFO_hIST has value 0x0040.

=item PNG_INFO_pHYs

PNG_INFO_pHYs has value 0x0080.

=item PNG_INFO_oFFs

PNG_INFO_oFFs has value 0x0100.

=item PNG_INFO_tIME

PNG_INFO_tIME has value 0x0200.

=item PNG_INFO_pCAL

PNG_INFO_pCAL has value 0x0400.

=item PNG_INFO_sRGB

PNG_INFO_sRGB has value 0x0800.

=item PNG_INFO_iCCP

PNG_INFO_iCCP has value 0x1000.

=item PNG_INFO_sPLT

PNG_INFO_sPLT has value 0x2000.

=item PNG_INFO_sCAL

PNG_INFO_sCAL has value 0x4000.

=item PNG_INFO_IDAT

PNG_INFO_IDAT has value 0x8000.

=item PNG_INFO_acTL

PNG_INFO_acTL has value 0x10000.

=item PNG_INFO_fcTL

PNG_INFO_fcTL has value 0x20000.

=item PNG_TRANSFORM_IDENTITY

PNG_TRANSFORM_IDENTITY has value 0x0000.

=item PNG_TRANSFORM_STRIP_16

PNG_TRANSFORM_STRIP_16 has value 0x0001.

=item PNG_TRANSFORM_STRIP_ALPHA

PNG_TRANSFORM_STRIP_ALPHA has value 0x0002.

=item PNG_TRANSFORM_PACKING

PNG_TRANSFORM_PACKING has value 0x0004.

=item PNG_TRANSFORM_PACKSWAP

PNG_TRANSFORM_PACKSWAP has value 0x0008.

=item PNG_TRANSFORM_EXPAND

PNG_TRANSFORM_EXPAND has value 0x0010.

=item PNG_TRANSFORM_INVERT_MONO

PNG_TRANSFORM_INVERT_MONO has value 0x0020.

=item PNG_TRANSFORM_SHIFT

PNG_TRANSFORM_SHIFT has value 0x0040.

=item PNG_TRANSFORM_BGR

PNG_TRANSFORM_BGR has value 0x0080.

=item PNG_TRANSFORM_SWAP_ALPHA

PNG_TRANSFORM_SWAP_ALPHA has value 0x0100.

=item PNG_TRANSFORM_SWAP_ENDIAN

PNG_TRANSFORM_SWAP_ENDIAN has value 0x0200.

=item PNG_TRANSFORM_INVERT_ALPHA

PNG_TRANSFORM_INVERT_ALPHA has value 0x0400.

=item PNG_TRANSFORM_STRIP_FILLER

PNG_TRANSFORM_STRIP_FILLER has value 0x0800.

=item PNG_TRANSFORM_STRIP_FILLER_BEFORE

PNG_TRANSFORM_STRIP_FILLER_BEFORE has value 0x0800.

=item PNG_TRANSFORM_STRIP_FILLER_AFTER

PNG_TRANSFORM_STRIP_FILLER_AFTER has value 0x1000.

=item PNG_TRANSFORM_GRAY_TO_RGB

PNG_TRANSFORM_GRAY_TO_RGB has value 0x2000.

=item PNG_TRANSFORM_EXPAND_16

PNG_TRANSFORM_EXPAND_16 has value 0x4000.

=item PNG_TRANSFORM_SCALE_16

PNG_TRANSFORM_SCALE_16 has value 0x8000.

=item PNG_FLAG_MNG_EMPTY_PLTE

PNG_FLAG_MNG_EMPTY_PLTE has value 0x01.

=item PNG_FLAG_MNG_FILTER_64

PNG_FLAG_MNG_FILTER_64 has value 0x04.

=item PNG_ALL_MNG_FEATURES

PNG_ALL_MNG_FEATURES has value 0x05.

=item PNG_ERROR_ACTION_NONE

PNG_ERROR_ACTION_NONE has value 1.

=item PNG_ERROR_ACTION_WARN

PNG_ERROR_ACTION_WARN has value 2.

=item PNG_ERROR_ACTION_ERROR

PNG_ERROR_ACTION_ERROR has value 3.

=item PNG_RGB_TO_GRAY_DEFAULT

PNG_RGB_TO_GRAY_DEFAULT has value (-1).

=item PNG_ALPHA_PNG

PNG_ALPHA_PNG has value 0.

=item PNG_ALPHA_STANDARD

PNG_ALPHA_STANDARD has value 1.

=item PNG_ALPHA_ASSOCIATED

PNG_ALPHA_ASSOCIATED has value 1.

=item PNG_ALPHA_PREMULTIPLIED

PNG_ALPHA_PREMULTIPLIED has value 1.

=item PNG_ALPHA_OPTIMIZED

PNG_ALPHA_OPTIMIZED has value 2.

=item PNG_ALPHA_BROKEN

PNG_ALPHA_BROKEN has value 3.

=item PNG_DEFAULT_sRGB

PNG_DEFAULT_sRGB has value -1.

=item PNG_GAMMA_MAC_18

PNG_GAMMA_MAC_18 has value -2.

=item PNG_GAMMA_sRGB

PNG_GAMMA_sRGB has value 220000.

=item PNG_GAMMA_LINEAR

PNG_GAMMA_LINEAR has value 100000.

=item PNG_FILLER_BEFORE

PNG_FILLER_BEFORE has value 0.

=item PNG_FILLER_AFTER

PNG_FILLER_AFTER has value 1.

=item PNG_BACKGROUND_GAMMA_UNKNOWN

PNG_BACKGROUND_GAMMA_UNKNOWN has value 0.

=item PNG_BACKGROUND_GAMMA_SCREEN

PNG_BACKGROUND_GAMMA_SCREEN has value 1.

=item PNG_BACKGROUND_GAMMA_FILE

PNG_BACKGROUND_GAMMA_FILE has value 2.

=item PNG_BACKGROUND_GAMMA_UNIQUE

PNG_BACKGROUND_GAMMA_UNIQUE has value 3.

=item PNG_READ_16_TO_8

PNG_READ_16_TO_8 has value SUPPORTED.

=item PNG_GAMMA_THRESHOLD

PNG_GAMMA_THRESHOLD has value (PNG_GAMMA_THRESHOLD_FIXED*.00001).

=item PNG_CRC_DEFAULT

PNG_CRC_DEFAULT has value 0.

=item PNG_CRC_ERROR_QUIT

PNG_CRC_ERROR_QUIT has value 1.

=item PNG_CRC_WARN_DISCARD

PNG_CRC_WARN_DISCARD has value 2.

=item PNG_CRC_WARN_USE

PNG_CRC_WARN_USE has value 3.

=item PNG_CRC_QUIET_USE

PNG_CRC_QUIET_USE has value 4.

=item PNG_CRC_NO_CHANGE

PNG_CRC_NO_CHANGE has value 5.

=item PNG_NO_FILTERS

PNG_NO_FILTERS has value 0x00.

=item PNG_FILTER_NONE

PNG_FILTER_NONE has value 0x08.

=item PNG_FILTER_SUB

PNG_FILTER_SUB has value 0x10.

=item PNG_FILTER_UP

PNG_FILTER_UP has value 0x20.

=item PNG_FILTER_AVG

PNG_FILTER_AVG has value 0x40.

=item PNG_FILTER_PAETH

PNG_FILTER_PAETH has value 0x80.

=item PNG_ALL_FILTERS

PNG_ALL_FILTERS has value (0x08 | 0x10 | 0x20 | 0x40 | 0x80).

=item PNG_FILTER_VALUE_NONE

PNG_FILTER_VALUE_NONE has value 0.

=item PNG_FILTER_VALUE_SUB

PNG_FILTER_VALUE_SUB has value 1.

=item PNG_FILTER_VALUE_UP

PNG_FILTER_VALUE_UP has value 2.

=item PNG_FILTER_VALUE_AVG

PNG_FILTER_VALUE_AVG has value 3.

=item PNG_FILTER_VALUE_PAETH

PNG_FILTER_VALUE_PAETH has value 4.

=item PNG_FILTER_HEURISTIC_DEFAULT

PNG_FILTER_HEURISTIC_DEFAULT has value 0.

=item PNG_FILTER_HEURISTIC_UNWEIGHTED

PNG_FILTER_HEURISTIC_UNWEIGHTED has value 1.

=item PNG_FILTER_HEURISTIC_WEIGHTED

PNG_FILTER_HEURISTIC_WEIGHTED has value 2.

=item PNG_DESTROY_WILL_FREE_DATA

PNG_DESTROY_WILL_FREE_DATA has value 1.

=item PNG_SET_WILL_FREE_DATA

PNG_SET_WILL_FREE_DATA has value 1.

=item PNG_USER_WILL_FREE_DATA

PNG_USER_WILL_FREE_DATA has value 2.

=item PNG_FREE_HIST

PNG_FREE_HIST has value 0x0008.

=item PNG_FREE_ICCP

PNG_FREE_ICCP has value 0x0010.

=item PNG_FREE_SPLT

PNG_FREE_SPLT has value 0x0020.

=item PNG_FREE_ROWS

PNG_FREE_ROWS has value 0x0040.

=item PNG_FREE_PCAL

PNG_FREE_PCAL has value 0x0080.

=item PNG_FREE_SCAL

PNG_FREE_SCAL has value 0x0100.

=item PNG_FREE_UNKN

PNG_FREE_UNKN has value 0x0200.

=item PNG_FREE_LIST

PNG_FREE_LIST has value 0x0400.

=item PNG_FREE_PLTE

PNG_FREE_PLTE has value 0x1000.

=item PNG_FREE_TRNS

PNG_FREE_TRNS has value 0x2000.

=item PNG_FREE_TEXT

PNG_FREE_TEXT has value 0x4000.

=item PNG_FREE_ALL

PNG_FREE_ALL has value 0x7fff.

=item PNG_FREE_MUL

PNG_FREE_MUL has value 0x4220.

=item PNG_HANDLE_CHUNK_AS_DEFAULT

PNG_HANDLE_CHUNK_AS_DEFAULT has value 0.

=item PNG_HANDLE_CHUNK_NEVER

PNG_HANDLE_CHUNK_NEVER has value 1.

=item PNG_HANDLE_CHUNK_IF_SAFE

PNG_HANDLE_CHUNK_IF_SAFE has value 2.

=item PNG_HANDLE_CHUNK_ALWAYS

PNG_HANDLE_CHUNK_ALWAYS has value 3.

=item PNG_IO_NONE

PNG_IO_NONE has value 0x0000.

=item PNG_IO_READING

PNG_IO_READING has value 0x0001.

=item PNG_IO_WRITING

PNG_IO_WRITING has value 0x0002.

=item PNG_IO_SIGNATURE

PNG_IO_SIGNATURE has value 0x0010.

=item PNG_IO_CHUNK_HDR

PNG_IO_CHUNK_HDR has value 0x0020.

=item PNG_IO_CHUNK_DATA

PNG_IO_CHUNK_DATA has value 0x0040.

=item PNG_IO_CHUNK_CRC

PNG_IO_CHUNK_CRC has value 0x0080.

=item PNG_IO_MASK_OP

PNG_IO_MASK_OP has value 0x000f.

=item PNG_IO_MASK_LOC

PNG_IO_MASK_LOC has value 0x00f0.

=item PNG_INTERLACE_ADAM7_PASSES

PNG_INTERLACE_ADAM7_PASSES has value 7.

=back

=head1 EXPORTS

Nothing is exported by default, but all the symbols in this module can
be exported on request. There is one export tag, 'all', which exports
all the symbols in this module:

    use Image::PNG::Const ':all';

=head1 ABOUT THIS MODULE

This Perl module was generated from C<png.h>.

=head1 AUTHOR

Ben Bullock, <bkb@cpan.org>

=head1 COPYRIGHT & LICENCE

The Image::PNG::Libpng package and associated files are copyright (C)
2014 Ben Bullock.

You can use, copy, modify and redistribute Image::PNG::Libpng and
associated files under the Perl Artistic Licence or the GNU General
Public Licence.

=head1 SUPPORT

=head2 Mailing list

There is a mailing list at
L<http://groups.google.com/group/perlimagepng>. It is not necessary to
be a member of the list to post messages to the list or participate in
discussions.

=head2 Reporting a bug

There is a bug tracker at L<https://github.com/benkasminbullock/image-png-libpng/issues>.



=cut

# Local variables:
# mode: perl
# End:
