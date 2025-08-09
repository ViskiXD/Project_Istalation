/*******************************************************************************************************************
Copyright (c) 2023 Cycling '74

The code that Max generates automatically and that end users are capable of
exporting and using, and any associated documentation files (the “Software”)
is a work of authorship for which Cycling '74 is the author and owner for
copyright purposes.

This Software is dual-licensed either under the terms of the Cycling '74
License for Max-Generated Code for Export, or alternatively under the terms
of the General Public License (GPL) Version 3. You may use the Software
according to either of these licenses as it is most appropriate for your
project on a case-by-case basis (proprietary or not).

A) Cycling '74 License for Max-Generated Code for Export

A license is hereby granted, free of charge, to any person obtaining a copy
of the Software (“Licensee”) to use, copy, modify, merge, publish, and
distribute copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The Software is licensed to Licensee for all uses that do not include the sale,
sublicensing, or commercial distribution of software that incorporates this
source code. This means that the Licensee is free to use this software for
educational, research, and prototyping purposes, to create musical or other
creative works with software that incorporates this source code, or any other
use that does not constitute selling software that makes use of this source
code. Commercial distribution also includes the packaging of free software with
other paid software, hardware, or software-provided commercial services.

For entities with UNDER $200k in annual revenue or funding, a license is hereby
granted, free of charge, for the sale, sublicensing, or commercial distribution
of software that incorporates this source code, for as long as the entity's
annual revenue remains below $200k annual revenue or funding.

For entities with OVER $200k in annual revenue or funding interested in the
sale, sublicensing, or commercial distribution of software that incorporates
this source code, please send inquiries to licensing@cycling74.com.

The above copyright notice and this license shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Please see
https://support.cycling74.com/hc/en-us/articles/10730637742483-RNBO-Export-Licensing-FAQ
for additional information

B) General Public License Version 3 (GPLv3)
Details of the GPLv3 license can be found at: https://www.gnu.org/licenses/gpl-3.0.html
*******************************************************************************************************************/

#include "RNBO_Common.h"
#include "RNBO_AudioSignal.h"

namespace RNBO {


#define trunc(x) ((Int)(x))

#if defined(__GNUC__) || defined(__clang__)
    #define RNBO_RESTRICT __restrict__
#elif defined(_MSC_VER)
    #define RNBO_RESTRICT __restrict
#endif

#define FIXEDSIZEARRAYINIT(...) { }

class rnbomatic : public PatcherInterfaceImpl {
public:

rnbomatic()
{
}

~rnbomatic()
{
}

rnbomatic* getTopLevelPatcher() {
    return this;
}

void cancelClockEvents()
{
    getEngine()->flushClockEvents(this, 870225038, false);
    getEngine()->flushClockEvents(this, -856764784, false);
    getEngine()->flushClockEvents(this, 6730127, false);
    getEngine()->flushClockEvents(this, 1927730382, false);
    getEngine()->flushClockEvents(this, -1720259695, false);
    getEngine()->flushClockEvents(this, -1503742003, false);
    getEngine()->flushClockEvents(this, 1733719949, false);
    getEngine()->flushClockEvents(this, -617739833, false);
    getEngine()->flushClockEvents(this, -640247092, false);
    getEngine()->flushClockEvents(this, 223247819, false);
    getEngine()->flushClockEvents(this, 1950237641, false);
    getEngine()->flushClockEvents(this, 1086742730, false);
    getEngine()->flushClockEvents(this, -1481234744, false);
    getEngine()->flushClockEvents(this, -834257525, false);
    getEngine()->flushClockEvents(this, -1697752436, false);
    getEngine()->flushClockEvents(this, 29237386, false);
    getEngine()->flushClockEvents(this, 892732297, false);
    getEngine()->flushClockEvents(this, 1777613927, false);
}

template <typename T> void listquicksort(T& arr, T& sortindices, Int l, Int h, bool ascending) {
    if (l < h) {
        Int p = (Int)(this->listpartition(arr, sortindices, l, h, ascending));
        this->listquicksort(arr, sortindices, l, p - 1, ascending);
        this->listquicksort(arr, sortindices, p + 1, h, ascending);
    }
}

template <typename T> Int listpartition(T& arr, T& sortindices, Int l, Int h, bool ascending) {
    number x = arr[(Index)h];
    Int i = (Int)(l - 1);

    for (Int j = (Int)(l); j <= h - 1; j++) {
        bool asc = (bool)((bool)(ascending) && arr[(Index)j] <= x);
        bool desc = (bool)((bool)(!(bool)(ascending)) && arr[(Index)j] >= x);

        if ((bool)(asc) || (bool)(desc)) {
            i++;
            this->listswapelements(arr, i, j);
            this->listswapelements(sortindices, i, j);
        }
    }

    i++;
    this->listswapelements(arr, i, h);
    this->listswapelements(sortindices, i, h);
    return i;
}

template <typename T> void listswapelements(T& arr, Int a, Int b) {
    auto tmp = arr[(Index)a];
    arr[(Index)a] = arr[(Index)b];
    arr[(Index)b] = tmp;
}

inline number linearinterp(number frac, number x, number y) {
    return x + (y - x) * frac;
}

inline number cubicinterp(number a, number w, number x, number y, number z) {
    number a1 = 1. + a;
    number aa = a * a1;
    number b = 1. - a;
    number b1 = 2. - a;
    number bb = b * b1;
    number fw = -.1666667 * bb * a;
    number fx = .5 * bb * a1;
    number fy = .5 * aa * b1;
    number fz = -.1666667 * aa * b;
    return w * fw + x * fx + y * fy + z * fz;
}

inline number fastcubicinterp(number a, number w, number x, number y, number z) {
    number a2 = a * a;
    number f0 = z - y - w + x;
    number f1 = w - x - f0;
    number f2 = y - w;
    number f3 = x;
    return f0 * a * a2 + f1 * a2 + f2 * a + f3;
}

inline number splineinterp(number a, number w, number x, number y, number z) {
    number a2 = a * a;
    number f0 = -0.5 * w + 1.5 * x - 1.5 * y + 0.5 * z;
    number f1 = w - 2.5 * x + 2 * y - 0.5 * z;
    number f2 = -0.5 * w + 0.5 * y;
    return f0 * a * a2 + f1 * a2 + f2 * a + x;
}

inline number spline6interp(number a, number y0, number y1, number y2, number y3, number y4, number y5) {
    number ym2py2 = y0 + y4;
    number ym1py1 = y1 + y3;
    number y2mym2 = y4 - y0;
    number y1mym1 = y3 - y1;
    number sixthym1py1 = (number)1 / (number)6.0 * ym1py1;
    number c0 = (number)1 / (number)120.0 * ym2py2 + (number)13 / (number)60.0 * ym1py1 + (number)11 / (number)20.0 * y2;
    number c1 = (number)1 / (number)24.0 * y2mym2 + (number)5 / (number)12.0 * y1mym1;
    number c2 = (number)1 / (number)12.0 * ym2py2 + sixthym1py1 - (number)1 / (number)2.0 * y2;
    number c3 = (number)1 / (number)12.0 * y2mym2 - (number)1 / (number)6.0 * y1mym1;
    number c4 = (number)1 / (number)24.0 * ym2py2 - sixthym1py1 + (number)1 / (number)4.0 * y2;
    number c5 = (number)1 / (number)120.0 * (y5 - y0) + (number)1 / (number)24.0 * (y1 - y4) + (number)1 / (number)12.0 * (y3 - y2);
    return ((((c5 * a + c4) * a + c3) * a + c2) * a + c1) * a + c0;
}

inline number cosT8(number r) {
    number t84 = 56.0;
    number t83 = 1680.0;
    number t82 = 20160.0;
    number t81 = 2.4801587302e-05;
    number t73 = 42.0;
    number t72 = 840.0;
    number t71 = 1.9841269841e-04;

    if (r < 0.785398163397448309615660845819875721 && r > -0.785398163397448309615660845819875721) {
        number rr = r * r;
        return 1.0 - rr * t81 * (t82 - rr * (t83 - rr * (t84 - rr)));
    } else if (r > 0.0) {
        r -= 1.57079632679489661923132169163975144;
        number rr = r * r;
        return -r * (1.0 - t71 * rr * (t72 - rr * (t73 - rr)));
    } else {
        r += 1.57079632679489661923132169163975144;
        number rr = r * r;
        return r * (1.0 - t71 * rr * (t72 - rr * (t73 - rr)));
    }
}

inline number cosineinterp(number frac, number x, number y) {
    number a2 = (1.0 - this->cosT8(frac * 3.14159265358979323846)) / (number)2.0;
    return x * (1.0 - a2) + y * a2;
}

number mstosamps(MillisecondTime ms) {
    return ms * this->sr * 0.001;
}

number samplerate() const {
    return this->sr;
}

Index vectorsize() const {
    return this->vs;
}

number maximum(number x, number y) {
    return (x < y ? y : x);
}

inline number safediv(number num, number denom) {
    return (denom == 0.0 ? 0.0 : num / denom);
}

number safepow(number base, number exponent) {
    return fixnan(rnbo_pow(base, exponent));
}

number scale(
    number x,
    number lowin,
    number hiin,
    number lowout,
    number highout,
    number pow
) {
    auto inscale = this->safediv(1., hiin - lowin);
    number outdiff = highout - lowout;
    number value = (x - lowin) * inscale;

    if (pow != 1) {
        if (value > 0)
            value = this->safepow(value, pow);
        else
            value = -this->safepow(-value, pow);
    }

    value = value * outdiff + lowout;
    return value;
}

number wrap(number x, number low, number high) {
    number lo;
    number hi;

    if (low == high)
        return low;

    if (low > high) {
        hi = low;
        lo = high;
    } else {
        lo = low;
        hi = high;
    }

    number range = hi - lo;

    if (x >= lo && x < hi)
        return x;

    if (range <= 0.000000001)
        return lo;

    long numWraps = (long)(trunc((x - lo) / range));
    numWraps = numWraps - ((x < lo ? 1 : 0));
    number result = x - range * numWraps;

    if (result >= hi)
        return result - range;
    else
        return result;
}

inline number intnum(const number value) {
    return trunc(value);
}

number __wrapped_op_clamp(number in1, number in2, number in3) {
    return (in1 > in3 ? in3 : (in1 < in2 ? in2 : in1));
}

MillisecondTime sampstoms(number samps) {
    return samps * 1000 / this->sr;
}

Index getNumMidiInputPorts() const {
    return 0;
}

void processMidiEvent(MillisecondTime , int , ConstByteArray , Index ) {}

Index getNumMidiOutputPorts() const {
    return 0;
}

void process(
    const SampleValue * const* inputs,
    Index numInputs,
    SampleValue * const* outputs,
    Index numOutputs,
    Index n
) {
    this->vs = n;
    this->updateTime(this->getEngine()->getCurrentTime());
    SampleValue * out1 = (numOutputs >= 1 && outputs[0] ? outputs[0] : this->dummyBuffer);
    SampleValue * out2 = (numOutputs >= 2 && outputs[1] ? outputs[1] : this->dummyBuffer);
    const SampleValue * in1 = (numInputs >= 1 && inputs[0] ? inputs[0] : this->zeroBuffer);
    const SampleValue * in2 = (numInputs >= 2 && inputs[1] ? inputs[1] : this->zeroBuffer);
    this->signaladder_01_perform(in2, in1, this->signals[1], n);
    this->dspexpr_08_perform(this->signals[1], this->dspexpr_08_in2, this->signals[0], n);
    this->dspexpr_09_perform(this->signals[0], this->signals[2], n);

    this->signalgate_02_perform(
        this->signalgate_02_onoff,
        this->signals[2],
        this->signals[0],
        this->signals[3],
        this->signals[4],
        this->signals[5],
        this->signals[6],
        this->signals[7],
        this->signals[8],
        this->signals[9],
        this->signals[10],
        this->signals[11],
        this->signals[12],
        this->dummyBuffer,
        n
    );

    this->signaladder_02_perform(
        this->signals[0],
        this->signals[6],
        this->signals[9],
        this->signals[11],
        this->signals[12],
        this->signals[6],
        n
    );

    this->signaladder_03_perform(
        this->signals[0],
        this->signals[6],
        this->signals[9],
        this->signals[11],
        this->signals[12],
        this->signals[6],
        n
    );

    this->gen_01_perform(
        this->signals[6],
        this->signals[6],
        this->gen_01_in3,
        this->gen_01_in4,
        this->gen_01_in5,
        this->gen_01_in6,
        this->gen_01_in7,
        this->gen_01_in8,
        this->signals[13],
        this->signals[14],
        n
    );

    this->signaladder_04_perform(this->signals[13], this->signals[14], this->signals[14], n);
    this->dspexpr_03_perform(this->signals[14], this->dspexpr_03_in2, this->signals[0], n);
    this->dspexpr_02_perform(this->signals[0], this->signals[13], n);

    this->signaladder_05_perform(
        this->signals[3],
        this->signals[5],
        this->signals[7],
        this->signals[9],
        this->signals[10],
        this->signals[12],
        this->signals[9],
        n
    );

    this->signaladder_06_perform(
        this->signals[3],
        this->signals[5],
        this->signals[7],
        this->signals[9],
        this->signals[10],
        this->signals[12],
        this->signals[9],
        n
    );

    this->gen_04_perform(
        this->signals[9],
        this->signals[9],
        this->gen_04_in3,
        this->gen_04_in4,
        this->gen_04_in5,
        this->gen_04_in6,
        this->gen_04_in7,
        this->gen_04_in8,
        this->signals[0],
        this->signals[15],
        n
    );

    this->signaladder_07_perform(this->signals[0], this->signals[15], this->signals[15], n);
    this->dspexpr_07_perform(this->signals[15], this->dspexpr_07_in2, this->signals[7], n);
    this->dspexpr_06_perform(this->signals[7], this->signals[0], n);

    this->signaladder_08_perform(
        this->signals[4],
        this->signals[5],
        this->signals[8],
        this->signals[10],
        this->signals[11],
        this->signals[12],
        this->signals[12],
        n
    );

    this->signaladder_09_perform(
        this->signals[4],
        this->signals[5],
        this->signals[8],
        this->signals[10],
        this->signals[11],
        this->signals[12],
        this->signals[12],
        n
    );

    this->gen_05_perform(
        this->signals[12],
        this->signals[12],
        this->gen_05_in3,
        this->gen_05_in4,
        this->gen_05_in5,
        this->gen_05_in6,
        this->gen_05_in7,
        this->gen_05_in8,
        this->signals[7],
        this->signals[3],
        n
    );

    this->signaladder_10_perform(this->signals[7], this->signals[3], this->signals[3], n);
    this->dspexpr_11_perform(this->signals[3], this->dspexpr_11_in2, this->signals[11], n);
    this->dspexpr_10_perform(this->signals[11], this->signals[7], n);
    this->dspexpr_01_perform(this->signals[2], this->dspexpr_01_in2, this->signals[11], n);

    this->signaladder_11_perform(
        this->signals[7],
        this->signals[0],
        this->signals[11],
        this->signals[13],
        this->signals[13],
        n
    );

    this->signalgate_01_perform(
        this->signalgate_01_onoff,
        this->signals[13],
        this->signals[2],
        this->signals[10],
        this->signals[8],
        this->signals[5],
        this->signals[4],
        this->signals[16],
        this->signals[17],
        this->signals[18],
        this->signals[19],
        this->signals[20],
        this->signals[21],
        this->signals[22],
        n
    );

    this->signaladder_12_perform(
        this->signals[4],
        this->signals[16],
        this->signals[17],
        this->signals[18],
        this->signals[19],
        this->signals[20],
        this->signals[21],
        this->signals[22],
        this->signals[22],
        n
    );

    this->gen_03_perform(
        this->signals[22],
        this->gen_03_in2,
        this->gen_03_in3,
        this->gen_03_in4,
        this->gen_03_in5,
        this->gen_03_in6,
        this->signals[11],
        this->signals[0],
        n
    );

    this->signaladder_13_perform(this->signals[11], this->signals[0], this->signals[20], n);
    this->dspexpr_05_perform(this->signals[20], this->dspexpr_05_in2, this->signals[21], n);

    this->signaladder_14_perform(
        this->signals[2],
        this->signals[10],
        this->signals[8],
        this->signals[5],
        this->signals[21],
        this->signals[21],
        n
    );

    this->dspexpr_04_perform(this->signals[21], this->signals[19], n);
    this->gen_02_perform(this->signals[19], this->gen_02_in2, this->gen_02_in3, out2, n);
    this->signalforwarder_01_perform(out2, out1, n);
    this->signaladder_15_perform(this->signals[11], this->signals[0], this->signals[0], n);
    this->metertilde_01_perform(this->signals[0], n);
    this->stackprotect_perform(n);
    this->globaltransport_advance();
    this->audioProcessSampleCount += this->vs;
}

void prepareToProcess(number sampleRate, Index maxBlockSize, bool force) {
    if (this->maxvs < maxBlockSize || !this->didAllocateSignals) {
        Index i;

        for (i = 0; i < 23; i++) {
            this->signals[i] = resizeSignal(this->signals[i], this->maxvs, maxBlockSize);
        }

        this->globaltransport_tempo = resizeSignal(this->globaltransport_tempo, this->maxvs, maxBlockSize);
        this->globaltransport_state = resizeSignal(this->globaltransport_state, this->maxvs, maxBlockSize);
        this->zeroBuffer = resizeSignal(this->zeroBuffer, this->maxvs, maxBlockSize);
        this->dummyBuffer = resizeSignal(this->dummyBuffer, this->maxvs, maxBlockSize);
        this->didAllocateSignals = true;
    }

    const bool sampleRateChanged = sampleRate != this->sr;
    const bool maxvsChanged = maxBlockSize != this->maxvs;
    const bool forceDSPSetup = sampleRateChanged || maxvsChanged || force;

    if (sampleRateChanged || maxvsChanged) {
        this->vs = maxBlockSize;
        this->maxvs = maxBlockSize;
        this->sr = sampleRate;
        this->invsr = 1 / sampleRate;
    }

    this->gen_01_dspsetup(forceDSPSetup);
    this->gen_04_dspsetup(forceDSPSetup);
    this->gen_05_dspsetup(forceDSPSetup);
    this->gen_03_dspsetup(forceDSPSetup);
    this->metertilde_01_dspsetup(forceDSPSetup);
    this->globaltransport_dspsetup(forceDSPSetup);

    if (sampleRateChanged)
        this->onSampleRateChanged(sampleRate);
}

void setProbingTarget(MessageTag id) {
    switch (id) {
    default:
        {
        this->setProbingIndex(-1);
        break;
        }
    }
}

void setProbingIndex(ProbingIndex ) {}

Index getProbingChannels(MessageTag outletId) const {
    RNBO_UNUSED(outletId);
    return 0;
}

DataRef* getDataRef(DataRefIndex index)  {
    switch (index) {
    case 0:
        {
        return addressOf(this->gen_01_delay_3_bufferobj);
        break;
        }
    case 1:
        {
        return addressOf(this->gen_01_delay_4_bufferobj);
        break;
        }
    case 2:
        {
        return addressOf(this->gen_03_delay_1_bufferobj);
        break;
        }
    case 3:
        {
        return addressOf(this->gen_03_delay_2_bufferobj);
        break;
        }
    case 4:
        {
        return addressOf(this->gen_03_delay_3_bufferobj);
        break;
        }
    case 5:
        {
        return addressOf(this->gen_03_delay_4_bufferobj);
        break;
        }
    case 6:
        {
        return addressOf(this->gen_03_delay_5_bufferobj);
        break;
        }
    case 7:
        {
        return addressOf(this->gen_03_delay_6_bufferobj);
        break;
        }
    case 8:
        {
        return addressOf(this->gen_03_delay_7_bufferobj);
        break;
        }
    case 9:
        {
        return addressOf(this->gen_03_delay_8_bufferobj);
        break;
        }
    case 10:
        {
        return addressOf(this->gen_03_delay_9_bufferobj);
        break;
        }
    case 11:
        {
        return addressOf(this->gen_03_delay_10_bufferobj);
        break;
        }
    case 12:
        {
        return addressOf(this->gen_03_delay_11_bufferobj);
        break;
        }
    case 13:
        {
        return addressOf(this->gen_03_delay_12_bufferobj);
        break;
        }
    case 14:
        {
        return addressOf(this->gen_03_delay_13_bufferobj);
        break;
        }
    case 15:
        {
        return addressOf(this->gen_03_delay_14_bufferobj);
        break;
        }
    case 16:
        {
        return addressOf(this->gen_03_delay_15_bufferobj);
        break;
        }
    case 17:
        {
        return addressOf(this->gen_03_delay_16_bufferobj);
        break;
        }
    case 18:
        {
        return addressOf(this->gen_03_delay_17_bufferobj);
        break;
        }
    case 19:
        {
        return addressOf(this->gen_03_delay_18_bufferobj);
        break;
        }
    case 20:
        {
        return addressOf(this->gen_03_delay_20_bufferobj);
        break;
        }
    case 21:
        {
        return addressOf(this->gen_03_delay_21_bufferobj);
        break;
        }
    case 22:
        {
        return addressOf(this->gen_03_delay_23_bufferobj);
        break;
        }
    case 23:
        {
        return addressOf(this->gen_03_delay_24_bufferobj);
        break;
        }
    case 24:
        {
        return addressOf(this->gen_03_delay_26_bufferobj);
        break;
        }
    case 25:
        {
        return addressOf(this->gen_03_delay_28_bufferobj);
        break;
        }
    case 26:
        {
        return addressOf(this->gen_03_delay_30_bufferobj);
        break;
        }
    case 27:
        {
        return addressOf(this->RNBODefaultSinus);
        break;
        }
    case 28:
        {
        return addressOf(this->gen_04_delay_3_bufferobj);
        break;
        }
    case 29:
        {
        return addressOf(this->gen_04_delay_4_bufferobj);
        break;
        }
    case 30:
        {
        return addressOf(this->gen_05_delay_3_bufferobj);
        break;
        }
    case 31:
        {
        return addressOf(this->gen_05_delay_4_bufferobj);
        break;
        }
    default:
        {
        return nullptr;
        }
    }
}

DataRefIndex getNumDataRefs() const {
    return 32;
}

void fillRNBODefaultSinus(DataRef& ref) {
    Float64BufferRef buffer;
    buffer = new Float64Buffer(ref);
    number bufsize = buffer->getSize();

    for (Index i = 0; i < bufsize; i++) {
        buffer[i] = rnbo_cos(i * 3.14159265358979323846 * 2. / bufsize);
    }
}

void fillDataRef(DataRefIndex index, DataRef& ref) {
    switch (index) {
    case 27:
        {
        this->fillRNBODefaultSinus(ref);
        break;
        }
    }
}

void zeroDataRef(DataRef& ref) {
    ref->setZero();
}

void processDataViewUpdate(DataRefIndex index, MillisecondTime time) {
    this->updateTime(time);

    if (index == 0) {
        this->gen_01_delay_3_buffer = new Float64Buffer(this->gen_01_delay_3_bufferobj);
    }

    if (index == 1) {
        this->gen_01_delay_4_buffer = new Float64Buffer(this->gen_01_delay_4_bufferobj);
    }

    if (index == 2) {
        this->gen_03_delay_1_buffer = new Float64Buffer(this->gen_03_delay_1_bufferobj);
    }

    if (index == 3) {
        this->gen_03_delay_2_buffer = new Float64Buffer(this->gen_03_delay_2_bufferobj);
    }

    if (index == 4) {
        this->gen_03_delay_3_buffer = new Float64Buffer(this->gen_03_delay_3_bufferobj);
    }

    if (index == 5) {
        this->gen_03_delay_4_buffer = new Float64Buffer(this->gen_03_delay_4_bufferobj);
    }

    if (index == 6) {
        this->gen_03_delay_5_buffer = new Float64Buffer(this->gen_03_delay_5_bufferobj);
    }

    if (index == 7) {
        this->gen_03_delay_6_buffer = new Float64Buffer(this->gen_03_delay_6_bufferobj);
    }

    if (index == 8) {
        this->gen_03_delay_7_buffer = new Float64Buffer(this->gen_03_delay_7_bufferobj);
    }

    if (index == 9) {
        this->gen_03_delay_8_buffer = new Float64Buffer(this->gen_03_delay_8_bufferobj);
    }

    if (index == 10) {
        this->gen_03_delay_9_buffer = new Float64Buffer(this->gen_03_delay_9_bufferobj);
    }

    if (index == 11) {
        this->gen_03_delay_10_buffer = new Float64Buffer(this->gen_03_delay_10_bufferobj);
    }

    if (index == 12) {
        this->gen_03_delay_11_buffer = new Float64Buffer(this->gen_03_delay_11_bufferobj);
    }

    if (index == 13) {
        this->gen_03_delay_12_buffer = new Float64Buffer(this->gen_03_delay_12_bufferobj);
    }

    if (index == 14) {
        this->gen_03_delay_13_buffer = new Float64Buffer(this->gen_03_delay_13_bufferobj);
    }

    if (index == 15) {
        this->gen_03_delay_14_buffer = new Float64Buffer(this->gen_03_delay_14_bufferobj);
    }

    if (index == 16) {
        this->gen_03_delay_15_buffer = new Float64Buffer(this->gen_03_delay_15_bufferobj);
    }

    if (index == 17) {
        this->gen_03_delay_16_buffer = new Float64Buffer(this->gen_03_delay_16_bufferobj);
    }

    if (index == 18) {
        this->gen_03_delay_17_buffer = new Float64Buffer(this->gen_03_delay_17_bufferobj);
    }

    if (index == 19) {
        this->gen_03_delay_18_buffer = new Float64Buffer(this->gen_03_delay_18_bufferobj);
    }

    if (index == 20) {
        this->gen_03_delay_20_buffer = new Float64Buffer(this->gen_03_delay_20_bufferobj);
    }

    if (index == 21) {
        this->gen_03_delay_21_buffer = new Float64Buffer(this->gen_03_delay_21_bufferobj);
    }

    if (index == 22) {
        this->gen_03_delay_23_buffer = new Float64Buffer(this->gen_03_delay_23_bufferobj);
    }

    if (index == 23) {
        this->gen_03_delay_24_buffer = new Float64Buffer(this->gen_03_delay_24_bufferobj);
    }

    if (index == 24) {
        this->gen_03_delay_26_buffer = new Float64Buffer(this->gen_03_delay_26_bufferobj);
    }

    if (index == 25) {
        this->gen_03_delay_28_buffer = new Float64Buffer(this->gen_03_delay_28_bufferobj);
    }

    if (index == 26) {
        this->gen_03_delay_30_buffer = new Float64Buffer(this->gen_03_delay_30_bufferobj);
    }

    if (index == 27) {
        this->gen_03_cycle_39_buffer = new Float64Buffer(this->RNBODefaultSinus);
        this->gen_03_cycle_54_buffer = new Float64Buffer(this->RNBODefaultSinus);
    }

    if (index == 28) {
        this->gen_04_delay_3_buffer = new Float64Buffer(this->gen_04_delay_3_bufferobj);
    }

    if (index == 29) {
        this->gen_04_delay_4_buffer = new Float64Buffer(this->gen_04_delay_4_bufferobj);
    }

    if (index == 30) {
        this->gen_05_delay_3_buffer = new Float64Buffer(this->gen_05_delay_3_bufferobj);
    }

    if (index == 31) {
        this->gen_05_delay_4_buffer = new Float64Buffer(this->gen_05_delay_4_bufferobj);
    }
}

void initialize() {
    this->gen_01_delay_3_bufferobj = initDataRef("gen_01_delay_3_bufferobj", true, nullptr, "buffer~");
    this->gen_01_delay_4_bufferobj = initDataRef("gen_01_delay_4_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_1_bufferobj = initDataRef("gen_03_delay_1_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_2_bufferobj = initDataRef("gen_03_delay_2_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_3_bufferobj = initDataRef("gen_03_delay_3_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_4_bufferobj = initDataRef("gen_03_delay_4_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_5_bufferobj = initDataRef("gen_03_delay_5_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_6_bufferobj = initDataRef("gen_03_delay_6_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_7_bufferobj = initDataRef("gen_03_delay_7_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_8_bufferobj = initDataRef("gen_03_delay_8_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_9_bufferobj = initDataRef("gen_03_delay_9_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_10_bufferobj = initDataRef("gen_03_delay_10_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_11_bufferobj = initDataRef("gen_03_delay_11_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_12_bufferobj = initDataRef("gen_03_delay_12_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_13_bufferobj = initDataRef("gen_03_delay_13_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_14_bufferobj = initDataRef("gen_03_delay_14_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_15_bufferobj = initDataRef("gen_03_delay_15_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_16_bufferobj = initDataRef("gen_03_delay_16_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_17_bufferobj = initDataRef("gen_03_delay_17_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_18_bufferobj = initDataRef("gen_03_delay_18_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_20_bufferobj = initDataRef("gen_03_delay_20_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_21_bufferobj = initDataRef("gen_03_delay_21_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_23_bufferobj = initDataRef("gen_03_delay_23_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_24_bufferobj = initDataRef("gen_03_delay_24_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_26_bufferobj = initDataRef("gen_03_delay_26_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_28_bufferobj = initDataRef("gen_03_delay_28_bufferobj", true, nullptr, "buffer~");
    this->gen_03_delay_30_bufferobj = initDataRef("gen_03_delay_30_bufferobj", true, nullptr, "buffer~");
    this->RNBODefaultSinus = initDataRef("RNBODefaultSinus", true, nullptr, "buffer~");
    this->gen_04_delay_3_bufferobj = initDataRef("gen_04_delay_3_bufferobj", true, nullptr, "buffer~");
    this->gen_04_delay_4_bufferobj = initDataRef("gen_04_delay_4_bufferobj", true, nullptr, "buffer~");
    this->gen_05_delay_3_bufferobj = initDataRef("gen_05_delay_3_bufferobj", true, nullptr, "buffer~");
    this->gen_05_delay_4_bufferobj = initDataRef("gen_05_delay_4_bufferobj", true, nullptr, "buffer~");
    this->assign_defaults();
    this->setState();
    this->gen_01_delay_3_bufferobj->setIndex(0);
    this->gen_01_delay_3_buffer = new Float64Buffer(this->gen_01_delay_3_bufferobj);
    this->gen_01_delay_4_bufferobj->setIndex(1);
    this->gen_01_delay_4_buffer = new Float64Buffer(this->gen_01_delay_4_bufferobj);
    this->gen_03_delay_1_bufferobj->setIndex(2);
    this->gen_03_delay_1_buffer = new Float64Buffer(this->gen_03_delay_1_bufferobj);
    this->gen_03_delay_2_bufferobj->setIndex(3);
    this->gen_03_delay_2_buffer = new Float64Buffer(this->gen_03_delay_2_bufferobj);
    this->gen_03_delay_3_bufferobj->setIndex(4);
    this->gen_03_delay_3_buffer = new Float64Buffer(this->gen_03_delay_3_bufferobj);
    this->gen_03_delay_4_bufferobj->setIndex(5);
    this->gen_03_delay_4_buffer = new Float64Buffer(this->gen_03_delay_4_bufferobj);
    this->gen_03_delay_5_bufferobj->setIndex(6);
    this->gen_03_delay_5_buffer = new Float64Buffer(this->gen_03_delay_5_bufferobj);
    this->gen_03_delay_6_bufferobj->setIndex(7);
    this->gen_03_delay_6_buffer = new Float64Buffer(this->gen_03_delay_6_bufferobj);
    this->gen_03_delay_7_bufferobj->setIndex(8);
    this->gen_03_delay_7_buffer = new Float64Buffer(this->gen_03_delay_7_bufferobj);
    this->gen_03_delay_8_bufferobj->setIndex(9);
    this->gen_03_delay_8_buffer = new Float64Buffer(this->gen_03_delay_8_bufferobj);
    this->gen_03_delay_9_bufferobj->setIndex(10);
    this->gen_03_delay_9_buffer = new Float64Buffer(this->gen_03_delay_9_bufferobj);
    this->gen_03_delay_10_bufferobj->setIndex(11);
    this->gen_03_delay_10_buffer = new Float64Buffer(this->gen_03_delay_10_bufferobj);
    this->gen_03_delay_11_bufferobj->setIndex(12);
    this->gen_03_delay_11_buffer = new Float64Buffer(this->gen_03_delay_11_bufferobj);
    this->gen_03_delay_12_bufferobj->setIndex(13);
    this->gen_03_delay_12_buffer = new Float64Buffer(this->gen_03_delay_12_bufferobj);
    this->gen_03_delay_13_bufferobj->setIndex(14);
    this->gen_03_delay_13_buffer = new Float64Buffer(this->gen_03_delay_13_bufferobj);
    this->gen_03_delay_14_bufferobj->setIndex(15);
    this->gen_03_delay_14_buffer = new Float64Buffer(this->gen_03_delay_14_bufferobj);
    this->gen_03_delay_15_bufferobj->setIndex(16);
    this->gen_03_delay_15_buffer = new Float64Buffer(this->gen_03_delay_15_bufferobj);
    this->gen_03_delay_16_bufferobj->setIndex(17);
    this->gen_03_delay_16_buffer = new Float64Buffer(this->gen_03_delay_16_bufferobj);
    this->gen_03_delay_17_bufferobj->setIndex(18);
    this->gen_03_delay_17_buffer = new Float64Buffer(this->gen_03_delay_17_bufferobj);
    this->gen_03_delay_18_bufferobj->setIndex(19);
    this->gen_03_delay_18_buffer = new Float64Buffer(this->gen_03_delay_18_bufferobj);
    this->gen_03_delay_20_bufferobj->setIndex(20);
    this->gen_03_delay_20_buffer = new Float64Buffer(this->gen_03_delay_20_bufferobj);
    this->gen_03_delay_21_bufferobj->setIndex(21);
    this->gen_03_delay_21_buffer = new Float64Buffer(this->gen_03_delay_21_bufferobj);
    this->gen_03_delay_23_bufferobj->setIndex(22);
    this->gen_03_delay_23_buffer = new Float64Buffer(this->gen_03_delay_23_bufferobj);
    this->gen_03_delay_24_bufferobj->setIndex(23);
    this->gen_03_delay_24_buffer = new Float64Buffer(this->gen_03_delay_24_bufferobj);
    this->gen_03_delay_26_bufferobj->setIndex(24);
    this->gen_03_delay_26_buffer = new Float64Buffer(this->gen_03_delay_26_bufferobj);
    this->gen_03_delay_28_bufferobj->setIndex(25);
    this->gen_03_delay_28_buffer = new Float64Buffer(this->gen_03_delay_28_bufferobj);
    this->gen_03_delay_30_bufferobj->setIndex(26);
    this->gen_03_delay_30_buffer = new Float64Buffer(this->gen_03_delay_30_bufferobj);
    this->RNBODefaultSinus->setIndex(27);
    this->gen_03_cycle_39_buffer = new Float64Buffer(this->RNBODefaultSinus);
    this->gen_03_cycle_54_buffer = new Float64Buffer(this->RNBODefaultSinus);
    this->gen_04_delay_3_bufferobj->setIndex(28);
    this->gen_04_delay_3_buffer = new Float64Buffer(this->gen_04_delay_3_bufferobj);
    this->gen_04_delay_4_bufferobj->setIndex(29);
    this->gen_04_delay_4_buffer = new Float64Buffer(this->gen_04_delay_4_bufferobj);
    this->gen_05_delay_3_bufferobj->setIndex(30);
    this->gen_05_delay_3_buffer = new Float64Buffer(this->gen_05_delay_3_bufferobj);
    this->gen_05_delay_4_bufferobj->setIndex(31);
    this->gen_05_delay_4_buffer = new Float64Buffer(this->gen_05_delay_4_bufferobj);
    this->initializeObjects();
    this->allocateDataRefs();
    this->startup();
}

Index getIsMuted()  {
    return this->isMuted;
}

void setIsMuted(Index v)  {
    this->isMuted = v;
}

void onSampleRateChanged(double ) {}

Index getPatcherSerial() const {
    return 0;
}

void getState(PatcherStateInterface& ) {}

void setState() {}

void getPreset(PatcherStateInterface& preset) {
    preset["__presetid"] = "rnbo";
    this->param_01_getPresetValue(getSubState(preset, "1st_Head"));
    this->param_02_getPresetValue(getSubState(preset, "Presets"));
    this->param_03_getPresetValue(getSubState(preset, "Reverb_Gain"));
    this->param_04_getPresetValue(getSubState(preset, "Echo_Rate"));
    this->param_05_getPresetValue(getSubState(preset, "2nd_Head"));
    this->param_06_getPresetValue(getSubState(preset, "FeedBack"));
    this->param_07_getPresetValue(getSubState(preset, "Lowpass_FeedBack"));
    this->param_08_getPresetValue(getSubState(preset, "Highpass_FeedBack"));
    this->param_09_getPresetValue(getSubState(preset, "Lowpass_Delay_FeedBack"));
    this->param_10_getPresetValue(getSubState(preset, "3rd_Head"));
    this->param_11_getPresetValue(getSubState(preset, "Lowpass_Global_FeedBack"));
}

void setPreset(MillisecondTime time, PatcherStateInterface& preset) {
    this->updateTime(time);
    this->param_01_setPresetValue(getSubState(preset, "1st_Head"));
    this->param_02_setPresetValue(getSubState(preset, "Presets"));
    this->param_03_setPresetValue(getSubState(preset, "Reverb_Gain"));
    this->param_04_setPresetValue(getSubState(preset, "Echo_Rate"));
    this->param_05_setPresetValue(getSubState(preset, "2nd_Head"));
    this->param_06_setPresetValue(getSubState(preset, "FeedBack"));
    this->param_07_setPresetValue(getSubState(preset, "Lowpass_FeedBack"));
    this->param_08_setPresetValue(getSubState(preset, "Highpass_FeedBack"));
    this->param_09_setPresetValue(getSubState(preset, "Lowpass_Delay_FeedBack"));
    this->param_10_setPresetValue(getSubState(preset, "3rd_Head"));
    this->param_11_setPresetValue(getSubState(preset, "Lowpass_Global_FeedBack"));
}

void setParameterValue(ParameterIndex index, ParameterValue v, MillisecondTime time) {
    this->updateTime(time);

    switch (index) {
    case 0:
        {
        this->param_01_value_set(v);
        break;
        }
    case 1:
        {
        this->param_02_value_set(v);
        break;
        }
    case 2:
        {
        this->param_03_value_set(v);
        break;
        }
    case 3:
        {
        this->param_04_value_set(v);
        break;
        }
    case 4:
        {
        this->param_05_value_set(v);
        break;
        }
    case 5:
        {
        this->param_06_value_set(v);
        break;
        }
    case 6:
        {
        this->param_07_value_set(v);
        break;
        }
    case 7:
        {
        this->param_08_value_set(v);
        break;
        }
    case 8:
        {
        this->param_09_value_set(v);
        break;
        }
    case 9:
        {
        this->param_10_value_set(v);
        break;
        }
    case 10:
        {
        this->param_11_value_set(v);
        break;
        }
    }
}

void processParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValue(index, value, time);
}

void processParameterBangEvent(ParameterIndex index, MillisecondTime time) {
    this->setParameterValue(index, this->getParameterValue(index), time);
}

void processNormalizedParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValueNormalized(index, value, time);
}

ParameterValue getParameterValue(ParameterIndex index)  {
    switch (index) {
    case 0:
        {
        return this->param_01_value;
        }
    case 1:
        {
        return this->param_02_value;
        }
    case 2:
        {
        return this->param_03_value;
        }
    case 3:
        {
        return this->param_04_value;
        }
    case 4:
        {
        return this->param_05_value;
        }
    case 5:
        {
        return this->param_06_value;
        }
    case 6:
        {
        return this->param_07_value;
        }
    case 7:
        {
        return this->param_08_value;
        }
    case 8:
        {
        return this->param_09_value;
        }
    case 9:
        {
        return this->param_10_value;
        }
    case 10:
        {
        return this->param_11_value;
        }
    default:
        {
        return 0;
        }
    }
}

ParameterIndex getNumSignalInParameters() const {
    return 0;
}

ParameterIndex getNumSignalOutParameters() const {
    return 0;
}

ParameterIndex getNumParameters() const {
    return 11;
}

ConstCharPointer getParameterName(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "1st_Head";
        }
    case 1:
        {
        return "Presets";
        }
    case 2:
        {
        return "Reverb_Gain";
        }
    case 3:
        {
        return "Echo_Rate";
        }
    case 4:
        {
        return "2nd_Head";
        }
    case 5:
        {
        return "FeedBack";
        }
    case 6:
        {
        return "Lowpass_FeedBack";
        }
    case 7:
        {
        return "Highpass_FeedBack";
        }
    case 8:
        {
        return "Lowpass_Delay_FeedBack";
        }
    case 9:
        {
        return "3rd_Head";
        }
    case 10:
        {
        return "Lowpass_Global_FeedBack";
        }
    default:
        {
        return "bogus";
        }
    }
}

ConstCharPointer getParameterId(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "1st_Head";
        }
    case 1:
        {
        return "Presets";
        }
    case 2:
        {
        return "Reverb_Gain";
        }
    case 3:
        {
        return "Echo_Rate";
        }
    case 4:
        {
        return "2nd_Head";
        }
    case 5:
        {
        return "FeedBack";
        }
    case 6:
        {
        return "Lowpass_FeedBack";
        }
    case 7:
        {
        return "Highpass_FeedBack";
        }
    case 8:
        {
        return "Lowpass_Delay_FeedBack";
        }
    case 9:
        {
        return "3rd_Head";
        }
    case 10:
        {
        return "Lowpass_Global_FeedBack";
        }
    default:
        {
        return "bogus";
        }
    }
}

void getParameterInfo(ParameterIndex index, ParameterInfo * info) const {
    {
        switch (index) {
        case 0:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 1:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 1;
            info->max = 12;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 2:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 50;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 3:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 127;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 4:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 5:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 6:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 127;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 7:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 127;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 8:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 127;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 9:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 10:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 127;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        }
    }
}

void sendParameter(ParameterIndex index, bool ignoreValue) {
    this->getEngine()->notifyParameterValueChanged(index, (ignoreValue ? 0 : this->getParameterValue(index)), ignoreValue);
}

ParameterValue applyStepsToNormalizedParameterValue(ParameterValue normalizedValue, int steps) const {
    if (steps == 1) {
        if (normalizedValue > 0) {
            normalizedValue = 1.;
        }
    } else {
        ParameterValue oneStep = (number)1. / (steps - 1);
        ParameterValue numberOfSteps = rnbo_fround(normalizedValue / oneStep * 1 / (number)1) * (number)1;
        normalizedValue = numberOfSteps * oneStep;
    }

    return normalizedValue;
}

ParameterValue convertToNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 0:
    case 4:
    case 5:
    case 9:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));
            ParameterValue normalizedValue = (value - 0) / (1 - 0);
            return normalizedValue;
        }
        }
    case 2:
        {
        {
            value = (value < 0 ? 0 : (value > 50 ? 50 : value));
            ParameterValue normalizedValue = (value - 0) / (50 - 0);
            return normalizedValue;
        }
        }
    case 3:
    case 6:
    case 7:
    case 8:
    case 10:
        {
        {
            value = (value < 0 ? 0 : (value > 127 ? 127 : value));
            ParameterValue normalizedValue = (value - 0) / (127 - 0);
            return normalizedValue;
        }
        }
    case 1:
        {
        {
            value = (value < 1 ? 1 : (value > 12 ? 12 : value));
            ParameterValue normalizedValue = (value - 1) / (12 - 1);
            return normalizedValue;
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue convertFromNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    value = (value < 0 ? 0 : (value > 1 ? 1 : value));

    switch (index) {
    case 0:
    case 4:
    case 5:
    case 9:
        {
        {
            {
                return 0 + value * (1 - 0);
            }
        }
        }
    case 2:
        {
        {
            {
                return 0 + value * (50 - 0);
            }
        }
        }
    case 3:
    case 6:
    case 7:
    case 8:
    case 10:
        {
        {
            {
                return 0 + value * (127 - 0);
            }
        }
        }
    case 1:
        {
        {
            {
                return 1 + value * (12 - 1);
            }
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue constrainParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 0:
        {
        return this->param_01_value_constrain(value);
        }
    case 1:
        {
        return this->param_02_value_constrain(value);
        }
    case 2:
        {
        return this->param_03_value_constrain(value);
        }
    case 3:
        {
        return this->param_04_value_constrain(value);
        }
    case 4:
        {
        return this->param_05_value_constrain(value);
        }
    case 5:
        {
        return this->param_06_value_constrain(value);
        }
    case 6:
        {
        return this->param_07_value_constrain(value);
        }
    case 7:
        {
        return this->param_08_value_constrain(value);
        }
    case 8:
        {
        return this->param_09_value_constrain(value);
        }
    case 9:
        {
        return this->param_10_value_constrain(value);
        }
    case 10:
        {
        return this->param_11_value_constrain(value);
        }
    default:
        {
        return value;
        }
    }
}

void scheduleParamInit(ParameterIndex index, Index order) {
    this->paramInitIndices->push(index);
    this->paramInitOrder->push(order);
}

void processParamInitEvents() {
    this->listquicksort(
        this->paramInitOrder,
        this->paramInitIndices,
        0,
        (int)(this->paramInitOrder->length - 1),
        true
    );

    for (Index i = 0; i < this->paramInitOrder->length; i++) {
        this->getEngine()->scheduleParameterBang(this->paramInitIndices[i], 0);
    }
}

void processClockEvent(MillisecondTime time, ClockId index, bool hasValue, ParameterValue value) {
    RNBO_UNUSED(hasValue);
    this->updateTime(time);

    switch (index) {
    case 870225038:
        {
        this->loadmess_01_startupbang_bang();
        break;
        }
    case -856764784:
        {
        this->loadmess_02_startupbang_bang();
        break;
        }
    case 6730127:
        {
        this->loadmess_03_startupbang_bang();
        break;
        }
    case 1927730382:
        {
        this->loadmess_04_startupbang_bang();
        break;
        }
    case -1720259695:
        {
        this->loadmess_05_startupbang_bang();
        break;
        }
    case -1503742003:
        {
        this->loadmess_06_startupbang_bang();
        break;
        }
    case 1733719949:
        {
        this->loadmess_07_startupbang_bang();
        break;
        }
    case -617739833:
        {
        this->loadmess_08_startupbang_bang();
        break;
        }
    case -640247092:
        {
        this->loadmess_09_startupbang_bang();
        break;
        }
    case 223247819:
        {
        this->loadmess_10_startupbang_bang();
        break;
        }
    case 1950237641:
        {
        this->loadmess_11_startupbang_bang();
        break;
        }
    case 1086742730:
        {
        this->loadmess_12_startupbang_bang();
        break;
        }
    case -1481234744:
        {
        this->loadmess_13_startupbang_bang();
        break;
        }
    case -834257525:
        {
        this->loadmess_14_startupbang_bang();
        break;
        }
    case -1697752436:
        {
        this->loadmess_15_startupbang_bang();
        break;
        }
    case 29237386:
        {
        this->loadmess_16_startupbang_bang();
        break;
        }
    case 892732297:
        {
        this->loadmess_17_startupbang_bang();
        break;
        }
    case 1777613927:
        {
        this->metertilde_01_value_set(value);
        break;
        }
    }
}

void processOutletAtCurrentTime(EngineLink* , OutletIndex , ParameterValue ) {}

void processOutletEvent(
    EngineLink* sender,
    OutletIndex index,
    ParameterValue value,
    MillisecondTime time
) {
    this->updateTime(time);
    this->processOutletAtCurrentTime(sender, index, value);
}

void processNumMessage(MessageTag tag, MessageTag objectId, MillisecondTime time, number payload) {
    this->updateTime(time);

    switch (tag) {
    case TAG("valin"):
        {
        if (TAG("number_obj-14") == objectId)
            this->numberobj_01_valin_set(payload);

        if (TAG("number_obj-17") == objectId)
            this->numberobj_02_valin_set(payload);

        if (TAG("number_obj-59") == objectId)
            this->numberobj_03_valin_set(payload);

        if (TAG("number_obj-61") == objectId)
            this->numberobj_04_valin_set(payload);

        if (TAG("number_obj-65") == objectId)
            this->numberobj_05_valin_set(payload);

        if (TAG("number_obj-64") == objectId)
            this->numberobj_06_valin_set(payload);

        if (TAG("number_obj-162") == objectId)
            this->numberobj_07_valin_set(payload);

        if (TAG("number_obj-30") == objectId)
            this->numberobj_08_valin_set(payload);

        if (TAG("number_obj-148") == objectId)
            this->numberobj_09_valin_set(payload);

        if (TAG("number_obj-55") == objectId)
            this->numberobj_10_valin_set(payload);

        if (TAG("number_obj-16") == objectId)
            this->numberobj_11_valin_set(payload);

        if (TAG("number_obj-79") == objectId)
            this->numberobj_12_valin_set(payload);

        if (TAG("number_obj-78") == objectId)
            this->numberobj_13_valin_set(payload);

        if (TAG("number_obj-60") == objectId)
            this->numberobj_14_valin_set(payload);

        if (TAG("number_obj-73") == objectId)
            this->numberobj_15_valin_set(payload);

        if (TAG("number_obj-72") == objectId)
            this->numberobj_16_valin_set(payload);

        if (TAG("number_obj-69") == objectId)
            this->numberobj_17_valin_set(payload);

        if (TAG("number_obj-80") == objectId)
            this->numberobj_18_valin_set(payload);

        if (TAG("number_obj-68") == objectId)
            this->numberobj_19_valin_set(payload);

        if (TAG("number_obj-38") == objectId)
            this->numberobj_20_valin_set(payload);

        if (TAG("number_obj-97") == objectId)
            this->numberobj_21_valin_set(payload);

        if (TAG("number_obj-163") == objectId)
            this->numberobj_22_valin_set(payload);

        if (TAG("slider_obj-39") == objectId)
            this->slider_01_valin_set(payload);

        if (TAG("number_obj-142") == objectId)
            this->numberobj_23_valin_set(payload);

        if (TAG("number_obj-20") == objectId)
            this->numberobj_24_valin_set(payload);

        if (TAG("number_obj-24") == objectId)
            this->numberobj_25_valin_set(payload);

        if (TAG("number_obj-62") == objectId)
            this->numberobj_26_valin_set(payload);

        if (TAG("number_obj-92") == objectId)
            this->numberobj_27_valin_set(payload);

        if (TAG("number_obj-91") == objectId)
            this->numberobj_28_valin_set(payload);

        if (TAG("number_obj-86") == objectId)
            this->numberobj_29_valin_set(payload);

        if (TAG("number_obj-85") == objectId)
            this->numberobj_30_valin_set(payload);

        if (TAG("number_obj-82") == objectId)
            this->numberobj_31_valin_set(payload);

        if (TAG("number_obj-81") == objectId)
            this->numberobj_32_valin_set(payload);

        if (TAG("number_obj-164") == objectId)
            this->numberobj_33_valin_set(payload);

        if (TAG("number_obj-99") == objectId)
            this->numberobj_34_valin_set(payload);

        if (TAG("number_obj-104") == objectId)
            this->numberobj_35_valin_set(payload);

        if (TAG("number_obj-34") == objectId)
            this->numberobj_36_valin_set(payload);

        if (TAG("number_obj-26") == objectId)
            this->numberobj_37_valin_set(payload);

        break;
        }
    case TAG("format"):
        {
        if (TAG("number_obj-14") == objectId)
            this->numberobj_01_format_set(payload);

        if (TAG("number_obj-17") == objectId)
            this->numberobj_02_format_set(payload);

        if (TAG("number_obj-59") == objectId)
            this->numberobj_03_format_set(payload);

        if (TAG("number_obj-61") == objectId)
            this->numberobj_04_format_set(payload);

        if (TAG("number_obj-65") == objectId)
            this->numberobj_05_format_set(payload);

        if (TAG("number_obj-64") == objectId)
            this->numberobj_06_format_set(payload);

        if (TAG("number_obj-162") == objectId)
            this->numberobj_07_format_set(payload);

        if (TAG("number_obj-30") == objectId)
            this->numberobj_08_format_set(payload);

        if (TAG("number_obj-148") == objectId)
            this->numberobj_09_format_set(payload);

        if (TAG("number_obj-55") == objectId)
            this->numberobj_10_format_set(payload);

        if (TAG("number_obj-16") == objectId)
            this->numberobj_11_format_set(payload);

        if (TAG("number_obj-79") == objectId)
            this->numberobj_12_format_set(payload);

        if (TAG("number_obj-78") == objectId)
            this->numberobj_13_format_set(payload);

        if (TAG("number_obj-60") == objectId)
            this->numberobj_14_format_set(payload);

        if (TAG("number_obj-73") == objectId)
            this->numberobj_15_format_set(payload);

        if (TAG("number_obj-72") == objectId)
            this->numberobj_16_format_set(payload);

        if (TAG("number_obj-69") == objectId)
            this->numberobj_17_format_set(payload);

        if (TAG("number_obj-80") == objectId)
            this->numberobj_18_format_set(payload);

        if (TAG("number_obj-68") == objectId)
            this->numberobj_19_format_set(payload);

        if (TAG("number_obj-38") == objectId)
            this->numberobj_20_format_set(payload);

        if (TAG("number_obj-97") == objectId)
            this->numberobj_21_format_set(payload);

        if (TAG("number_obj-163") == objectId)
            this->numberobj_22_format_set(payload);

        if (TAG("number_obj-142") == objectId)
            this->numberobj_23_format_set(payload);

        if (TAG("number_obj-20") == objectId)
            this->numberobj_24_format_set(payload);

        if (TAG("number_obj-24") == objectId)
            this->numberobj_25_format_set(payload);

        if (TAG("number_obj-62") == objectId)
            this->numberobj_26_format_set(payload);

        if (TAG("number_obj-92") == objectId)
            this->numberobj_27_format_set(payload);

        if (TAG("number_obj-91") == objectId)
            this->numberobj_28_format_set(payload);

        if (TAG("number_obj-86") == objectId)
            this->numberobj_29_format_set(payload);

        if (TAG("number_obj-85") == objectId)
            this->numberobj_30_format_set(payload);

        if (TAG("number_obj-82") == objectId)
            this->numberobj_31_format_set(payload);

        if (TAG("number_obj-81") == objectId)
            this->numberobj_32_format_set(payload);

        if (TAG("number_obj-164") == objectId)
            this->numberobj_33_format_set(payload);

        if (TAG("number_obj-99") == objectId)
            this->numberobj_34_format_set(payload);

        if (TAG("number_obj-104") == objectId)
            this->numberobj_35_format_set(payload);

        if (TAG("number_obj-34") == objectId)
            this->numberobj_36_format_set(payload);

        if (TAG("number_obj-26") == objectId)
            this->numberobj_37_format_set(payload);

        break;
        }
    }
}

void processListMessage(MessageTag , MessageTag , MillisecondTime , const list& ) {}

void processBangMessage(MessageTag tag, MessageTag objectId, MillisecondTime time) {
    this->updateTime(time);

    switch (tag) {
    case TAG("startupbang"):
        {
        if (TAG("loadmess_obj-173") == objectId)
            this->loadmess_01_startupbang_bang();

        if (TAG("loadmess_obj-171") == objectId)
            this->loadmess_02_startupbang_bang();

        if (TAG("loadmess_obj-172") == objectId)
            this->loadmess_03_startupbang_bang();

        if (TAG("loadmess_obj-149") == objectId)
            this->loadmess_04_startupbang_bang();

        if (TAG("loadmess_obj-170") == objectId)
            this->loadmess_05_startupbang_bang();

        if (TAG("loadmess_obj-150") == objectId)
            this->loadmess_06_startupbang_bang();

        if (TAG("loadmess_obj-174") == objectId)
            this->loadmess_07_startupbang_bang();

        if (TAG("loadmess_obj-169") == objectId)
            this->loadmess_08_startupbang_bang();

        if (TAG("loadmess_obj-152") == objectId)
            this->loadmess_09_startupbang_bang();

        if (TAG("loadmess_obj-153") == objectId)
            this->loadmess_10_startupbang_bang();

        if (TAG("loadmess_obj-155") == objectId)
            this->loadmess_11_startupbang_bang();

        if (TAG("loadmess_obj-154") == objectId)
            this->loadmess_12_startupbang_bang();

        if (TAG("loadmess_obj-166") == objectId)
            this->loadmess_13_startupbang_bang();

        if (TAG("loadmess_obj-47") == objectId)
            this->loadmess_14_startupbang_bang();

        if (TAG("loadmess_obj-175") == objectId)
            this->loadmess_15_startupbang_bang();

        if (TAG("loadmess_obj-93") == objectId)
            this->loadmess_16_startupbang_bang();

        if (TAG("loadmess_obj-102") == objectId)
            this->loadmess_17_startupbang_bang();

        break;
        }
    }
}

MessageTagInfo resolveTag(MessageTag tag) const {
    switch (tag) {
    case TAG("valout"):
        {
        return "valout";
        }
    case TAG("number_obj-14"):
        {
        return "number_obj-14";
        }
    case TAG("setup"):
        {
        return "setup";
        }
    case TAG("number_obj-17"):
        {
        return "number_obj-17";
        }
    case TAG("number_obj-59"):
        {
        return "number_obj-59";
        }
    case TAG("number_obj-61"):
        {
        return "number_obj-61";
        }
    case TAG("number_obj-65"):
        {
        return "number_obj-65";
        }
    case TAG("number_obj-64"):
        {
        return "number_obj-64";
        }
    case TAG("number_obj-162"):
        {
        return "number_obj-162";
        }
    case TAG("number_obj-30"):
        {
        return "number_obj-30";
        }
    case TAG("number_obj-148"):
        {
        return "number_obj-148";
        }
    case TAG("number_obj-55"):
        {
        return "number_obj-55";
        }
    case TAG("number_obj-16"):
        {
        return "number_obj-16";
        }
    case TAG("number_obj-79"):
        {
        return "number_obj-79";
        }
    case TAG("number_obj-78"):
        {
        return "number_obj-78";
        }
    case TAG("number_obj-60"):
        {
        return "number_obj-60";
        }
    case TAG("number_obj-73"):
        {
        return "number_obj-73";
        }
    case TAG("number_obj-72"):
        {
        return "number_obj-72";
        }
    case TAG("number_obj-69"):
        {
        return "number_obj-69";
        }
    case TAG("meter~_obj-100"):
        {
        return "meter~_obj-100";
        }
    case TAG("peak"):
        {
        return "peak";
        }
    case TAG("number_obj-80"):
        {
        return "number_obj-80";
        }
    case TAG("number_obj-68"):
        {
        return "number_obj-68";
        }
    case TAG("number_obj-38"):
        {
        return "number_obj-38";
        }
    case TAG("number_obj-97"):
        {
        return "number_obj-97";
        }
    case TAG("number_obj-163"):
        {
        return "number_obj-163";
        }
    case TAG("slider_obj-39"):
        {
        return "slider_obj-39";
        }
    case TAG("number_obj-142"):
        {
        return "number_obj-142";
        }
    case TAG("number_obj-20"):
        {
        return "number_obj-20";
        }
    case TAG("number_obj-24"):
        {
        return "number_obj-24";
        }
    case TAG("number_obj-62"):
        {
        return "number_obj-62";
        }
    case TAG("number_obj-92"):
        {
        return "number_obj-92";
        }
    case TAG("number_obj-91"):
        {
        return "number_obj-91";
        }
    case TAG("number_obj-86"):
        {
        return "number_obj-86";
        }
    case TAG("number_obj-85"):
        {
        return "number_obj-85";
        }
    case TAG("number_obj-82"):
        {
        return "number_obj-82";
        }
    case TAG("number_obj-81"):
        {
        return "number_obj-81";
        }
    case TAG("number_obj-164"):
        {
        return "number_obj-164";
        }
    case TAG("number_obj-99"):
        {
        return "number_obj-99";
        }
    case TAG("number_obj-104"):
        {
        return "number_obj-104";
        }
    case TAG("number_obj-34"):
        {
        return "number_obj-34";
        }
    case TAG("number_obj-26"):
        {
        return "number_obj-26";
        }
    case TAG("valin"):
        {
        return "valin";
        }
    case TAG("format"):
        {
        return "format";
        }
    case TAG("startupbang"):
        {
        return "startupbang";
        }
    case TAG("loadmess_obj-173"):
        {
        return "loadmess_obj-173";
        }
    case TAG("loadmess_obj-171"):
        {
        return "loadmess_obj-171";
        }
    case TAG("loadmess_obj-172"):
        {
        return "loadmess_obj-172";
        }
    case TAG("loadmess_obj-149"):
        {
        return "loadmess_obj-149";
        }
    case TAG("loadmess_obj-170"):
        {
        return "loadmess_obj-170";
        }
    case TAG("loadmess_obj-150"):
        {
        return "loadmess_obj-150";
        }
    case TAG("loadmess_obj-174"):
        {
        return "loadmess_obj-174";
        }
    case TAG("loadmess_obj-169"):
        {
        return "loadmess_obj-169";
        }
    case TAG("loadmess_obj-152"):
        {
        return "loadmess_obj-152";
        }
    case TAG("loadmess_obj-153"):
        {
        return "loadmess_obj-153";
        }
    case TAG("loadmess_obj-155"):
        {
        return "loadmess_obj-155";
        }
    case TAG("loadmess_obj-154"):
        {
        return "loadmess_obj-154";
        }
    case TAG("loadmess_obj-166"):
        {
        return "loadmess_obj-166";
        }
    case TAG("loadmess_obj-47"):
        {
        return "loadmess_obj-47";
        }
    case TAG("loadmess_obj-175"):
        {
        return "loadmess_obj-175";
        }
    case TAG("loadmess_obj-93"):
        {
        return "loadmess_obj-93";
        }
    case TAG("loadmess_obj-102"):
        {
        return "loadmess_obj-102";
        }
    }

    return "";
}

MessageIndex getNumMessages() const {
    return 0;
}

const MessageInfo& getMessageInfo(MessageIndex index) const {
    switch (index) {

    }

    return NullMessageInfo;
}

protected:

void param_01_value_set(number v) {
    v = this->param_01_value_constrain(v);
    this->param_01_value = v;
    this->sendParameter(0, false);

    if (this->param_01_value != this->param_01_lastValue) {
        this->getEngine()->presetTouched();
        this->param_01_lastValue = this->param_01_value;
    }

    this->numberobj_07_value_set(v);
}

void param_02_value_set(number v) {
    v = this->param_02_value_constrain(v);
    this->param_02_value = v;
    this->sendParameter(1, false);

    if (this->param_02_value != this->param_02_lastValue) {
        this->getEngine()->presetTouched();
        this->param_02_lastValue = this->param_02_value;
    }

    this->numberobj_08_value_set(v);
}

void param_03_value_set(number v) {
    v = this->param_03_value_constrain(v);
    this->param_03_value = v;
    this->sendParameter(2, false);

    if (this->param_03_value != this->param_03_lastValue) {
        this->getEngine()->presetTouched();
        this->param_03_lastValue = this->param_03_value;
    }

    this->numberobj_09_value_set(v);
}

void param_04_value_set(number v) {
    v = this->param_04_value_constrain(v);
    this->param_04_value = v;
    this->sendParameter(3, false);

    if (this->param_04_value != this->param_04_lastValue) {
        this->getEngine()->presetTouched();
        this->param_04_lastValue = this->param_04_value;
    }

    this->slider_01_input_set(v);
}

void param_05_value_set(number v) {
    v = this->param_05_value_constrain(v);
    this->param_05_value = v;
    this->sendParameter(4, false);

    if (this->param_05_value != this->param_05_lastValue) {
        this->getEngine()->presetTouched();
        this->param_05_lastValue = this->param_05_value;
    }

    this->numberobj_22_value_set(v);
}

void param_06_value_set(number v) {
    v = this->param_06_value_constrain(v);
    this->param_06_value = v;
    this->sendParameter(5, false);

    if (this->param_06_value != this->param_06_lastValue) {
        this->getEngine()->presetTouched();
        this->param_06_lastValue = this->param_06_value;
    }

    this->numberobj_24_value_set(v);
}

void param_07_value_set(number v) {
    v = this->param_07_value_constrain(v);
    this->param_07_value = v;
    this->sendParameter(6, false);

    if (this->param_07_value != this->param_07_lastValue) {
        this->getEngine()->presetTouched();
        this->param_07_lastValue = this->param_07_value;
    }

    this->numberobj_25_value_set(v);
}

void param_08_value_set(number v) {
    v = this->param_08_value_constrain(v);
    this->param_08_value = v;
    this->sendParameter(7, false);

    if (this->param_08_value != this->param_08_lastValue) {
        this->getEngine()->presetTouched();
        this->param_08_lastValue = this->param_08_value;
    }

    this->numberobj_26_value_set(v);
}

void param_09_value_set(number v) {
    v = this->param_09_value_constrain(v);
    this->param_09_value = v;
    this->sendParameter(8, false);

    if (this->param_09_value != this->param_09_lastValue) {
        this->getEngine()->presetTouched();
        this->param_09_lastValue = this->param_09_value;
    }

    this->numberobj_34_value_set(v);
}

void param_10_value_set(number v) {
    v = this->param_10_value_constrain(v);
    this->param_10_value = v;
    this->sendParameter(9, false);

    if (this->param_10_value != this->param_10_lastValue) {
        this->getEngine()->presetTouched();
        this->param_10_lastValue = this->param_10_value;
    }

    this->numberobj_33_value_set(v);
}

void param_11_value_set(number v) {
    v = this->param_11_value_constrain(v);
    this->param_11_value = v;
    this->sendParameter(10, false);

    if (this->param_11_value != this->param_11_lastValue) {
        this->getEngine()->presetTouched();
        this->param_11_lastValue = this->param_11_value;
    }

    this->numberobj_37_value_set(v);
}

void numberobj_01_valin_set(number v) {
    this->numberobj_01_value_set(v);
}

void numberobj_01_format_set(number v) {
    this->numberobj_01_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_02_valin_set(number v) {
    this->numberobj_02_value_set(v);
}

void numberobj_02_format_set(number v) {
    this->numberobj_02_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_03_valin_set(number v) {
    this->numberobj_03_value_set(v);
}

void numberobj_03_format_set(number v) {
    this->numberobj_03_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_04_valin_set(number v) {
    this->numberobj_04_value_set(v);
}

void numberobj_04_format_set(number v) {
    this->numberobj_04_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_05_valin_set(number v) {
    this->numberobj_05_value_set(v);
}

void numberobj_05_format_set(number v) {
    this->numberobj_05_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_06_valin_set(number v) {
    this->numberobj_06_value_set(v);
}

void numberobj_06_format_set(number v) {
    this->numberobj_06_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_07_valin_set(number v) {
    this->numberobj_07_value_set(v);
}

void numberobj_07_format_set(number v) {
    this->numberobj_07_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void loadmess_01_startupbang_bang() {
    this->loadmess_01_message_bang();
}

void loadmess_02_startupbang_bang() {
    this->loadmess_02_message_bang();
}

void numberobj_08_valin_set(number v) {
    this->numberobj_08_value_set(v);
}

void numberobj_08_format_set(number v) {
    this->numberobj_08_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_09_valin_set(number v) {
    this->numberobj_09_value_set(v);
}

void numberobj_09_format_set(number v) {
    this->numberobj_09_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void loadmess_03_startupbang_bang() {
    this->loadmess_03_message_bang();
}

void loadmess_04_startupbang_bang() {
    this->loadmess_04_message_bang();
}

void numberobj_10_valin_set(number v) {
    this->numberobj_10_value_set(v);
}

void numberobj_10_format_set(number v) {
    this->numberobj_10_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_11_valin_set(number v) {
    this->numberobj_11_value_set(v);
}

void numberobj_11_format_set(number v) {
    this->numberobj_11_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_12_valin_set(number v) {
    this->numberobj_12_value_set(v);
}

void numberobj_12_format_set(number v) {
    this->numberobj_12_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_13_valin_set(number v) {
    this->numberobj_13_value_set(v);
}

void numberobj_13_format_set(number v) {
    this->numberobj_13_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_14_valin_set(number v) {
    this->numberobj_14_value_set(v);
}

void numberobj_14_format_set(number v) {
    this->numberobj_14_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_15_valin_set(number v) {
    this->numberobj_15_value_set(v);
}

void numberobj_15_format_set(number v) {
    this->numberobj_15_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_16_valin_set(number v) {
    this->numberobj_16_value_set(v);
}

void numberobj_16_format_set(number v) {
    this->numberobj_16_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void loadmess_05_startupbang_bang() {
    this->loadmess_05_message_bang();
}

void numberobj_17_valin_set(number v) {
    this->numberobj_17_value_set(v);
}

void numberobj_17_format_set(number v) {
    this->numberobj_17_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_18_valin_set(number v) {
    this->numberobj_18_value_set(v);
}

void numberobj_18_format_set(number v) {
    this->numberobj_18_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_19_valin_set(number v) {
    this->numberobj_19_value_set(v);
}

void numberobj_19_format_set(number v) {
    this->numberobj_19_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_20_valin_set(number v) {
    this->numberobj_20_value_set(v);
}

void numberobj_20_format_set(number v) {
    this->numberobj_20_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void loadmess_06_startupbang_bang() {
    this->loadmess_06_message_bang();
}

void numberobj_21_valin_set(number v) {
    this->numberobj_21_value_set(v);
}

void numberobj_21_format_set(number v) {
    this->numberobj_21_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void loadmess_07_startupbang_bang() {
    this->loadmess_07_message_bang();
}

void numberobj_22_valin_set(number v) {
    this->numberobj_22_value_set(v);
}

void numberobj_22_format_set(number v) {
    this->numberobj_22_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void slider_01_valin_set(number v) {
    this->slider_01_value_set(v);
}

void loadmess_08_startupbang_bang() {
    this->loadmess_08_message_bang();
}

void numberobj_23_valin_set(number v) {
    this->numberobj_23_value_set(v);
}

void numberobj_23_format_set(number v) {
    this->numberobj_23_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void loadmess_09_startupbang_bang() {
    this->loadmess_09_message_bang();
}

void loadmess_10_startupbang_bang() {
    this->loadmess_10_message_bang();
}

void loadmess_11_startupbang_bang() {
    this->loadmess_11_message_bang();
}

void numberobj_24_valin_set(number v) {
    this->numberobj_24_value_set(v);
}

void numberobj_24_format_set(number v) {
    this->numberobj_24_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void loadmess_12_startupbang_bang() {
    this->loadmess_12_message_bang();
}

void loadmess_13_startupbang_bang() {
    this->loadmess_13_message_bang();
}

void numberobj_25_valin_set(number v) {
    this->numberobj_25_value_set(v);
}

void numberobj_25_format_set(number v) {
    this->numberobj_25_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void loadmess_14_startupbang_bang() {
    this->loadmess_14_message_bang();
}

void numberobj_26_valin_set(number v) {
    this->numberobj_26_value_set(v);
}

void numberobj_26_format_set(number v) {
    this->numberobj_26_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_27_valin_set(number v) {
    this->numberobj_27_value_set(v);
}

void numberobj_27_format_set(number v) {
    this->numberobj_27_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_28_valin_set(number v) {
    this->numberobj_28_value_set(v);
}

void numberobj_28_format_set(number v) {
    this->numberobj_28_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_29_valin_set(number v) {
    this->numberobj_29_value_set(v);
}

void numberobj_29_format_set(number v) {
    this->numberobj_29_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_30_valin_set(number v) {
    this->numberobj_30_value_set(v);
}

void numberobj_30_format_set(number v) {
    this->numberobj_30_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_31_valin_set(number v) {
    this->numberobj_31_value_set(v);
}

void numberobj_31_format_set(number v) {
    this->numberobj_31_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_32_valin_set(number v) {
    this->numberobj_32_value_set(v);
}

void numberobj_32_format_set(number v) {
    this->numberobj_32_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_33_valin_set(number v) {
    this->numberobj_33_value_set(v);
}

void numberobj_33_format_set(number v) {
    this->numberobj_33_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void loadmess_15_startupbang_bang() {
    this->loadmess_15_message_bang();
}

void loadmess_16_startupbang_bang() {
    this->loadmess_16_message_bang();
}

void numberobj_34_valin_set(number v) {
    this->numberobj_34_value_set(v);
}

void numberobj_34_format_set(number v) {
    this->numberobj_34_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void loadmess_17_startupbang_bang() {
    this->loadmess_17_message_bang();
}

void numberobj_35_valin_set(number v) {
    this->numberobj_35_value_set(v);
}

void numberobj_35_format_set(number v) {
    this->numberobj_35_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_36_valin_set(number v) {
    this->numberobj_36_value_set(v);
}

void numberobj_36_format_set(number v) {
    this->numberobj_36_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void numberobj_37_valin_set(number v) {
    this->numberobj_37_value_set(v);
}

void numberobj_37_format_set(number v) {
    this->numberobj_37_currentFormat = trunc((v > 6 ? 6 : (v < 0 ? 0 : v)));
}

void metertilde_01_value_set(number ) {}

number msToSamps(MillisecondTime ms, number sampleRate) {
    return ms * sampleRate * 0.001;
}

MillisecondTime sampsToMs(SampleIndex samps) {
    return samps * (this->invsr * 1000);
}

Index getMaxBlockSize() const {
    return this->maxvs;
}

number getSampleRate() const {
    return this->sr;
}

bool hasFixedVectorSize() const {
    return false;
}

Index getNumInputChannels() const {
    return 2;
}

Index getNumOutputChannels() const {
    return 2;
}

void allocateDataRefs() {
    this->gen_03_cycle_39_buffer->requestSize(16384, 1);
    this->gen_03_cycle_39_buffer->setSampleRate(this->sr);
    this->gen_03_cycle_54_buffer->requestSize(16384, 1);
    this->gen_03_cycle_54_buffer->setSampleRate(this->sr);
    this->gen_01_delay_3_buffer = this->gen_01_delay_3_buffer->allocateIfNeeded();

    if (this->gen_01_delay_3_bufferobj->hasRequestedSize()) {
        if (this->gen_01_delay_3_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_delay_3_bufferobj);

        this->getEngine()->sendDataRefUpdated(0);
    }

    this->gen_01_delay_4_buffer = this->gen_01_delay_4_buffer->allocateIfNeeded();

    if (this->gen_01_delay_4_bufferobj->hasRequestedSize()) {
        if (this->gen_01_delay_4_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_delay_4_bufferobj);

        this->getEngine()->sendDataRefUpdated(1);
    }

    this->gen_03_delay_1_buffer = this->gen_03_delay_1_buffer->allocateIfNeeded();

    if (this->gen_03_delay_1_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_1_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_1_bufferobj);

        this->getEngine()->sendDataRefUpdated(2);
    }

    this->gen_03_delay_2_buffer = this->gen_03_delay_2_buffer->allocateIfNeeded();

    if (this->gen_03_delay_2_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_2_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_2_bufferobj);

        this->getEngine()->sendDataRefUpdated(3);
    }

    this->gen_03_delay_3_buffer = this->gen_03_delay_3_buffer->allocateIfNeeded();

    if (this->gen_03_delay_3_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_3_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_3_bufferobj);

        this->getEngine()->sendDataRefUpdated(4);
    }

    this->gen_03_delay_4_buffer = this->gen_03_delay_4_buffer->allocateIfNeeded();

    if (this->gen_03_delay_4_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_4_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_4_bufferobj);

        this->getEngine()->sendDataRefUpdated(5);
    }

    this->gen_03_delay_5_buffer = this->gen_03_delay_5_buffer->allocateIfNeeded();

    if (this->gen_03_delay_5_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_5_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_5_bufferobj);

        this->getEngine()->sendDataRefUpdated(6);
    }

    this->gen_03_delay_6_buffer = this->gen_03_delay_6_buffer->allocateIfNeeded();

    if (this->gen_03_delay_6_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_6_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_6_bufferobj);

        this->getEngine()->sendDataRefUpdated(7);
    }

    this->gen_03_delay_7_buffer = this->gen_03_delay_7_buffer->allocateIfNeeded();

    if (this->gen_03_delay_7_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_7_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_7_bufferobj);

        this->getEngine()->sendDataRefUpdated(8);
    }

    this->gen_03_delay_8_buffer = this->gen_03_delay_8_buffer->allocateIfNeeded();

    if (this->gen_03_delay_8_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_8_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_8_bufferobj);

        this->getEngine()->sendDataRefUpdated(9);
    }

    this->gen_03_delay_9_buffer = this->gen_03_delay_9_buffer->allocateIfNeeded();

    if (this->gen_03_delay_9_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_9_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_9_bufferobj);

        this->getEngine()->sendDataRefUpdated(10);
    }

    this->gen_03_delay_10_buffer = this->gen_03_delay_10_buffer->allocateIfNeeded();

    if (this->gen_03_delay_10_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_10_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_10_bufferobj);

        this->getEngine()->sendDataRefUpdated(11);
    }

    this->gen_03_delay_11_buffer = this->gen_03_delay_11_buffer->allocateIfNeeded();

    if (this->gen_03_delay_11_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_11_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_11_bufferobj);

        this->getEngine()->sendDataRefUpdated(12);
    }

    this->gen_03_delay_12_buffer = this->gen_03_delay_12_buffer->allocateIfNeeded();

    if (this->gen_03_delay_12_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_12_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_12_bufferobj);

        this->getEngine()->sendDataRefUpdated(13);
    }

    this->gen_03_delay_13_buffer = this->gen_03_delay_13_buffer->allocateIfNeeded();

    if (this->gen_03_delay_13_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_13_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_13_bufferobj);

        this->getEngine()->sendDataRefUpdated(14);
    }

    this->gen_03_delay_14_buffer = this->gen_03_delay_14_buffer->allocateIfNeeded();

    if (this->gen_03_delay_14_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_14_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_14_bufferobj);

        this->getEngine()->sendDataRefUpdated(15);
    }

    this->gen_03_delay_15_buffer = this->gen_03_delay_15_buffer->allocateIfNeeded();

    if (this->gen_03_delay_15_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_15_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_15_bufferobj);

        this->getEngine()->sendDataRefUpdated(16);
    }

    this->gen_03_delay_16_buffer = this->gen_03_delay_16_buffer->allocateIfNeeded();

    if (this->gen_03_delay_16_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_16_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_16_bufferobj);

        this->getEngine()->sendDataRefUpdated(17);
    }

    this->gen_03_delay_17_buffer = this->gen_03_delay_17_buffer->allocateIfNeeded();

    if (this->gen_03_delay_17_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_17_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_17_bufferobj);

        this->getEngine()->sendDataRefUpdated(18);
    }

    this->gen_03_delay_18_buffer = this->gen_03_delay_18_buffer->allocateIfNeeded();

    if (this->gen_03_delay_18_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_18_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_18_bufferobj);

        this->getEngine()->sendDataRefUpdated(19);
    }

    this->gen_03_delay_20_buffer = this->gen_03_delay_20_buffer->allocateIfNeeded();

    if (this->gen_03_delay_20_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_20_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_20_bufferobj);

        this->getEngine()->sendDataRefUpdated(20);
    }

    this->gen_03_delay_21_buffer = this->gen_03_delay_21_buffer->allocateIfNeeded();

    if (this->gen_03_delay_21_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_21_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_21_bufferobj);

        this->getEngine()->sendDataRefUpdated(21);
    }

    this->gen_03_delay_23_buffer = this->gen_03_delay_23_buffer->allocateIfNeeded();

    if (this->gen_03_delay_23_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_23_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_23_bufferobj);

        this->getEngine()->sendDataRefUpdated(22);
    }

    this->gen_03_delay_24_buffer = this->gen_03_delay_24_buffer->allocateIfNeeded();

    if (this->gen_03_delay_24_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_24_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_24_bufferobj);

        this->getEngine()->sendDataRefUpdated(23);
    }

    this->gen_03_delay_26_buffer = this->gen_03_delay_26_buffer->allocateIfNeeded();

    if (this->gen_03_delay_26_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_26_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_26_bufferobj);

        this->getEngine()->sendDataRefUpdated(24);
    }

    this->gen_03_delay_28_buffer = this->gen_03_delay_28_buffer->allocateIfNeeded();

    if (this->gen_03_delay_28_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_28_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_28_bufferobj);

        this->getEngine()->sendDataRefUpdated(25);
    }

    this->gen_03_delay_30_buffer = this->gen_03_delay_30_buffer->allocateIfNeeded();

    if (this->gen_03_delay_30_bufferobj->hasRequestedSize()) {
        if (this->gen_03_delay_30_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_03_delay_30_bufferobj);

        this->getEngine()->sendDataRefUpdated(26);
    }

    this->gen_03_cycle_39_buffer = this->gen_03_cycle_39_buffer->allocateIfNeeded();
    this->gen_03_cycle_54_buffer = this->gen_03_cycle_54_buffer->allocateIfNeeded();

    if (this->RNBODefaultSinus->hasRequestedSize()) {
        if (this->RNBODefaultSinus->wantsFill())
            this->fillRNBODefaultSinus(this->RNBODefaultSinus);

        this->getEngine()->sendDataRefUpdated(27);
    }

    this->gen_04_delay_3_buffer = this->gen_04_delay_3_buffer->allocateIfNeeded();

    if (this->gen_04_delay_3_bufferobj->hasRequestedSize()) {
        if (this->gen_04_delay_3_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_04_delay_3_bufferobj);

        this->getEngine()->sendDataRefUpdated(28);
    }

    this->gen_04_delay_4_buffer = this->gen_04_delay_4_buffer->allocateIfNeeded();

    if (this->gen_04_delay_4_bufferobj->hasRequestedSize()) {
        if (this->gen_04_delay_4_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_04_delay_4_bufferobj);

        this->getEngine()->sendDataRefUpdated(29);
    }

    this->gen_05_delay_3_buffer = this->gen_05_delay_3_buffer->allocateIfNeeded();

    if (this->gen_05_delay_3_bufferobj->hasRequestedSize()) {
        if (this->gen_05_delay_3_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_05_delay_3_bufferobj);

        this->getEngine()->sendDataRefUpdated(30);
    }

    this->gen_05_delay_4_buffer = this->gen_05_delay_4_buffer->allocateIfNeeded();

    if (this->gen_05_delay_4_bufferobj->hasRequestedSize()) {
        if (this->gen_05_delay_4_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_05_delay_4_bufferobj);

        this->getEngine()->sendDataRefUpdated(31);
    }
}

void initializeObjects() {
    this->gen_01_history_1_init();
    this->gen_01_history_2_init();
    this->gen_01_delay_3_init();
    this->gen_01_delay_4_init();
    this->gen_01_history_5_init();
    this->gen_01_history_6_init();
    this->gen_01_history_7_init();
    this->gen_01_history_8_init();
    this->gen_01_history_9_init();
    this->gen_01_history_10_init();
    this->gen_01_history_11_init();
    this->gen_01_history_12_init();
    this->numberobj_01_init();
    this->numberobj_02_init();
    this->numberobj_03_init();
    this->numberobj_04_init();
    this->numberobj_05_init();
    this->numberobj_06_init();
    this->numberobj_07_init();
    this->gen_02_history_1_init();
    this->gen_02_history_2_init();
    this->numberobj_08_init();
    this->numberobj_09_init();
    this->gen_03_delay_1_init();
    this->gen_03_delay_2_init();
    this->gen_03_delay_3_init();
    this->gen_03_delay_4_init();
    this->gen_03_delay_5_init();
    this->gen_03_delay_6_init();
    this->gen_03_delay_7_init();
    this->gen_03_delay_8_init();
    this->gen_03_delay_9_init();
    this->gen_03_delay_10_init();
    this->gen_03_delay_11_init();
    this->gen_03_delay_12_init();
    this->gen_03_delay_13_init();
    this->gen_03_delay_14_init();
    this->gen_03_delay_15_init();
    this->gen_03_delay_16_init();
    this->gen_03_delay_17_init();
    this->gen_03_delay_18_init();
    this->gen_03_history_19_init();
    this->gen_03_delay_20_init();
    this->gen_03_delay_21_init();
    this->gen_03_history_22_init();
    this->gen_03_delay_23_init();
    this->gen_03_delay_24_init();
    this->gen_03_history_25_init();
    this->gen_03_delay_26_init();
    this->gen_03_history_27_init();
    this->gen_03_delay_28_init();
    this->gen_03_history_29_init();
    this->gen_03_delay_30_init();
    this->gen_03_history_31_init();
    this->gen_03_history_32_init();
    this->gen_03_history_33_init();
    this->gen_04_history_1_init();
    this->gen_04_history_2_init();
    this->gen_04_delay_3_init();
    this->gen_04_delay_4_init();
    this->gen_04_history_5_init();
    this->gen_04_history_6_init();
    this->gen_04_history_7_init();
    this->gen_04_history_8_init();
    this->gen_04_history_9_init();
    this->gen_04_history_10_init();
    this->gen_04_history_11_init();
    this->gen_04_history_12_init();
    this->numberobj_10_init();
    this->numberobj_11_init();
    this->numberobj_12_init();
    this->numberobj_13_init();
    this->numberobj_14_init();
    this->numberobj_15_init();
    this->numberobj_16_init();
    this->numberobj_17_init();
    this->metertilde_01_init();
    this->numberobj_18_init();
    this->numberobj_19_init();
    this->numberobj_20_init();
    this->numberobj_21_init();
    this->numberobj_22_init();
    this->numberobj_23_init();
    this->numberobj_24_init();
    this->numberobj_25_init();
    this->gen_05_history_1_init();
    this->gen_05_history_2_init();
    this->gen_05_delay_3_init();
    this->gen_05_delay_4_init();
    this->gen_05_history_5_init();
    this->gen_05_history_6_init();
    this->gen_05_history_7_init();
    this->gen_05_history_8_init();
    this->gen_05_history_9_init();
    this->gen_05_history_10_init();
    this->gen_05_history_11_init();
    this->gen_05_history_12_init();
    this->numberobj_26_init();
    this->numberobj_27_init();
    this->numberobj_28_init();
    this->numberobj_29_init();
    this->numberobj_30_init();
    this->numberobj_31_init();
    this->numberobj_32_init();
    this->numberobj_33_init();
    this->numberobj_34_init();
    this->numberobj_35_init();
    this->numberobj_36_init();
    this->numberobj_37_init();
}

void sendOutlet(OutletIndex index, ParameterValue value) {
    this->getEngine()->sendOutlet(this, index, value);
}

void startup() {
    this->updateTime(this->getEngine()->getCurrentTime());
    this->getEngine()->scheduleClockEvent(this, 870225038, 0 + this->_currentTime);;
    this->getEngine()->scheduleClockEvent(this, -856764784, 0 + this->_currentTime);;
    this->getEngine()->scheduleClockEvent(this, 6730127, 0 + this->_currentTime);;
    this->getEngine()->scheduleClockEvent(this, 1927730382, 0 + this->_currentTime);;
    this->getEngine()->scheduleClockEvent(this, -1720259695, 0 + this->_currentTime);;
    this->getEngine()->scheduleClockEvent(this, -1503742003, 0 + this->_currentTime);;
    this->getEngine()->scheduleClockEvent(this, 1733719949, 0 + this->_currentTime);;
    this->getEngine()->scheduleClockEvent(this, -617739833, 0 + this->_currentTime);;
    this->getEngine()->scheduleClockEvent(this, -640247092, 0 + this->_currentTime);;
    this->getEngine()->scheduleClockEvent(this, 223247819, 0 + this->_currentTime);;
    this->getEngine()->scheduleClockEvent(this, 1950237641, 0 + this->_currentTime);;
    this->getEngine()->scheduleClockEvent(this, 1086742730, 0 + this->_currentTime);;
    this->getEngine()->scheduleClockEvent(this, -1481234744, 0 + this->_currentTime);;
    this->getEngine()->scheduleClockEvent(this, -834257525, 0 + this->_currentTime);;
    this->getEngine()->scheduleClockEvent(this, -1697752436, 0 + this->_currentTime);;
    this->getEngine()->scheduleClockEvent(this, 29237386, 0 + this->_currentTime);;
    this->getEngine()->scheduleClockEvent(this, 892732297, 0 + this->_currentTime);;

    {
        this->scheduleParamInit(0, 0);
    }

    {
        this->scheduleParamInit(1, 0);
    }

    {
        this->scheduleParamInit(2, 0);
    }

    {
        this->scheduleParamInit(3, 0);
    }

    {
        this->scheduleParamInit(4, 0);
    }

    {
        this->scheduleParamInit(5, 0);
    }

    {
        this->scheduleParamInit(6, 0);
    }

    {
        this->scheduleParamInit(7, 0);
    }

    {
        this->scheduleParamInit(8, 0);
    }

    {
        this->scheduleParamInit(9, 0);
    }

    {
        this->scheduleParamInit(10, 0);
    }

    this->processParamInitEvents();
}

number param_01_value_constrain(number v) const {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void dspexpr_03_in2_set(number v) {
    this->dspexpr_03_in2 = v;
}

void numberobj_07_output_set(number v) {
    this->dspexpr_03_in2_set(v);
}

void numberobj_07_value_set(number v) {
    this->numberobj_07_value_setter(v);
    v = this->numberobj_07_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 2) {
        localvalue = 2;
    }

    if (this->numberobj_07_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-162"), localvalue, this->_currentTime);
    this->numberobj_07_output_set(localvalue);
}

number param_02_value_constrain(number v) const {
    v = (v > 12 ? 12 : (v < 1 ? 1 : v));
    return v;
}

void signalgate_02_onoff_set(number v) {
    this->signalgate_02_onoff = v;
}

void signalgate_01_onoff_set(number v) {
    this->signalgate_01_onoff = v;
}

void numberobj_08_output_set(number v) {
    this->signalgate_02_onoff_set(v);
    this->signalgate_01_onoff_set(v);
}

void numberobj_08_value_set(number v) {
    this->numberobj_08_value_setter(v);
    v = this->numberobj_08_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 12) {
        localvalue = 12;
    }

    if (this->numberobj_08_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-30"), localvalue, this->_currentTime);
    this->numberobj_08_output_set(localvalue);
}

number param_03_value_constrain(number v) const {
    v = (v > 50 ? 50 : (v < 0 ? 0 : v));
    return v;
}

void dspexpr_05_in2_set(number v) {
    this->dspexpr_05_in2 = v;
}

void numberobj_09_output_set(number v) {
    this->dspexpr_05_in2_set(v);
}

void numberobj_09_value_set(number v) {
    this->numberobj_09_value_setter(v);
    v = this->numberobj_09_value;
    number localvalue = v;

    if (this->numberobj_09_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-148"), localvalue, this->_currentTime);
    this->numberobj_09_output_set(localvalue);
}

number param_04_value_constrain(number v) const {
    v = (v > 127 ? 127 : (v < 0 ? 0 : v));
    return v;
}

void gen_05_in3_set(number v) {
    this->gen_05_in3 = v;
}

void numberobj_27_output_set(number v) {
    this->gen_05_in3_set(v);
}

void numberobj_27_value_set(number v) {
    this->numberobj_27_value_setter(v);
    v = this->numberobj_27_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 189) {
        localvalue = 189;
    }

    if ((bool)(true) && localvalue > 487) {
        localvalue = 487;
    }

    if (this->numberobj_27_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-92"), localvalue, this->_currentTime);
    this->numberobj_27_output_set(localvalue);
}

void scale_03_out_set(const list& v) {
    this->scale_03_out = jsCreateListCopy(v);

    {
        number converted = (v->length > 0 ? v[0] : 0);
        this->numberobj_27_value_set(converted);
    }
}

void scale_03_input_set(const list& v) {
    this->scale_03_input = jsCreateListCopy(v);
    list tmp = {};

    for (Index i = 0; i < v->length; i++) {
        tmp->push(this->scale(
            v[(Index)i],
            this->scale_03_inlow,
            this->scale_03_inhigh,
            this->scale_03_outlow,
            this->scale_03_outhigh,
            this->scale_03_power
        ));
    }

    this->scale_03_out_set(tmp);
}

void numberobj_20_output_set(number ) {}

void numberobj_20_value_set(number v) {
    this->numberobj_20_value_setter(v);
    v = this->numberobj_20_value;
    number localvalue = v;

    if (this->numberobj_20_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-38"), localvalue, this->_currentTime);
    this->numberobj_20_output_set(localvalue);
}

void gen_04_in3_set(number v) {
    this->gen_04_in3 = v;
}

void numberobj_12_output_set(number v) {
    this->gen_04_in3_set(v);
}

void numberobj_12_value_set(number v) {
    this->numberobj_12_value_setter(v);
    v = this->numberobj_12_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 131) {
        localvalue = 131;
    }

    if ((bool)(true) && localvalue > 336) {
        localvalue = 336;
    }

    if (this->numberobj_12_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-79"), localvalue, this->_currentTime);
    this->numberobj_12_output_set(localvalue);
}

void scale_02_out_set(const list& v) {
    this->scale_02_out = jsCreateListCopy(v);

    {
        number converted = (v->length > 0 ? v[0] : 0);
        this->numberobj_12_value_set(converted);
    }
}

void scale_02_input_set(const list& v) {
    this->scale_02_input = jsCreateListCopy(v);
    list tmp = {};

    for (Index i = 0; i < v->length; i++) {
        tmp->push(this->scale(
            v[(Index)i],
            this->scale_02_inlow,
            this->scale_02_inhigh,
            this->scale_02_outlow,
            this->scale_02_outhigh,
            this->scale_02_power
        ));
    }

    this->scale_02_out_set(tmp);
}

void gen_01_in3_set(number v) {
    this->gen_01_in3 = v;
}

void numberobj_01_output_set(number v) {
    this->gen_01_in3_set(v);
}

void numberobj_01_value_set(number v) {
    this->numberobj_01_value_setter(v);
    v = this->numberobj_01_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 69) {
        localvalue = 69;
    }

    if ((bool)(true) && localvalue > 177) {
        localvalue = 177;
    }

    if (this->numberobj_01_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-14"), localvalue, this->_currentTime);
    this->numberobj_01_output_set(localvalue);
}

void scale_01_out_set(const list& v) {
    this->scale_01_out = jsCreateListCopy(v);

    {
        number converted = (v->length > 0 ? v[0] : 0);
        this->numberobj_01_value_set(converted);
    }
}

void scale_01_input_set(const list& v) {
    this->scale_01_input = jsCreateListCopy(v);
    list tmp = {};

    for (Index i = 0; i < v->length; i++) {
        tmp->push(this->scale(
            v[(Index)i],
            this->scale_01_inlow,
            this->scale_01_inhigh,
            this->scale_01_outlow,
            this->scale_01_outhigh,
            this->scale_01_power
        ));
    }

    this->scale_01_out_set(tmp);
}

void slider_01_output_set(number v) {
    {
        list converted = {v};
        this->scale_03_input_set(converted);
    }

    this->numberobj_20_value_set(v);

    {
        list converted = {v};
        this->scale_02_input_set(converted);
    }

    {
        list converted = {v};
        this->scale_01_input_set(converted);
    }
}

void slider_01_value_set(number v) {
    this->slider_01_value = v;
    number value;

    {
        value = this->scale(v, 0, 128, 0, 0 + 128, 1) * 1;
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("slider_obj-39"), v, this->_currentTime);
    this->slider_01_output_set(value);
}

void slider_01_input_set(number v) {
    this->slider_01_value_set(this->scale(
        this->__wrapped_op_clamp(this->safediv(v, 1), 0, 0 + 128),
        0,
        0 + 128,
        0,
        128,
        1
    ));
}

number param_05_value_constrain(number v) const {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void dspexpr_07_in2_set(number v) {
    this->dspexpr_07_in2 = v;
}

void numberobj_22_output_set(number v) {
    this->dspexpr_07_in2_set(v);
}

void numberobj_22_value_set(number v) {
    this->numberobj_22_value_setter(v);
    v = this->numberobj_22_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 2) {
        localvalue = 2;
    }

    if (this->numberobj_22_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-163"), localvalue, this->_currentTime);
    this->numberobj_22_output_set(localvalue);
}

number param_06_value_constrain(number v) const {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void gen_05_in4_set(number v) {
    this->gen_05_in4 = v;
}

void numberobj_28_output_set(number v) {
    this->gen_05_in4_set(v);
}

void numberobj_28_value_set(number v) {
    this->numberobj_28_value_setter(v);
    v = this->numberobj_28_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 0.999) {
        localvalue = 0.999;
    }

    if (this->numberobj_28_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-91"), localvalue, this->_currentTime);
    this->numberobj_28_output_set(localvalue);
}

void gen_04_in4_set(number v) {
    this->gen_04_in4 = v;
}

void numberobj_13_output_set(number v) {
    this->gen_04_in4_set(v);
}

void numberobj_13_value_set(number v) {
    this->numberobj_13_value_setter(v);
    v = this->numberobj_13_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 0.999) {
        localvalue = 0.999;
    }

    if (this->numberobj_13_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-78"), localvalue, this->_currentTime);
    this->numberobj_13_output_set(localvalue);
}

void gen_01_in4_set(number v) {
    this->gen_01_in4 = v;
}

void numberobj_02_output_set(number v) {
    this->gen_01_in4_set(v);
}

void numberobj_02_value_set(number v) {
    this->numberobj_02_value_setter(v);
    v = this->numberobj_02_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 0.999) {
        localvalue = 0.999;
    }

    if (this->numberobj_02_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-17"), localvalue, this->_currentTime);
    this->numberobj_02_output_set(localvalue);
}

void numberobj_24_output_set(number v) {
    this->numberobj_28_value_set(v);
    this->numberobj_13_value_set(v);
    this->numberobj_02_value_set(v);
}

void numberobj_24_value_set(number v) {
    this->numberobj_24_value_setter(v);
    v = this->numberobj_24_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 0.85) {
        localvalue = 0.85;
    }

    if (this->numberobj_24_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-20"), localvalue, this->_currentTime);
    this->numberobj_24_output_set(localvalue);
}

number param_07_value_constrain(number v) const {
    v = (v > 127 ? 127 : (v < 0 ? 0 : v));
    return v;
}

void gen_05_in5_set(number v) {
    this->gen_05_in5 = v;
}

void numberobj_29_output_set(number v) {
    this->gen_05_in5_set(v);
}

void numberobj_29_value_set(number v) {
    this->numberobj_29_value_setter(v);
    v = this->numberobj_29_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_29_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-86"), localvalue, this->_currentTime);
    this->numberobj_29_output_set(localvalue);
}

void gen_04_in5_set(number v) {
    this->gen_04_in5 = v;
}

void numberobj_15_output_set(number v) {
    this->gen_04_in5_set(v);
}

void numberobj_15_value_set(number v) {
    this->numberobj_15_value_setter(v);
    v = this->numberobj_15_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_15_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-73"), localvalue, this->_currentTime);
    this->numberobj_15_output_set(localvalue);
}

void gen_01_in5_set(number v) {
    this->gen_01_in5 = v;
}

void numberobj_03_output_set(number v) {
    this->gen_01_in5_set(v);
}

void numberobj_03_value_set(number v) {
    this->numberobj_03_value_setter(v);
    v = this->numberobj_03_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_03_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-59"), localvalue, this->_currentTime);
    this->numberobj_03_output_set(localvalue);
}

void numberobj_25_output_set(number v) {
    this->numberobj_29_value_set(v);
    this->numberobj_15_value_set(v);
    this->numberobj_03_value_set(v);
}

void numberobj_25_value_set(number v) {
    this->numberobj_25_value_setter(v);
    v = this->numberobj_25_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 1) {
        localvalue = 1;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_25_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-24"), localvalue, this->_currentTime);
    this->numberobj_25_output_set(localvalue);
}

number param_08_value_constrain(number v) const {
    v = (v > 127 ? 127 : (v < 0 ? 0 : v));
    return v;
}

void gen_05_in6_set(number v) {
    this->gen_05_in6 = v;
}

void numberobj_30_output_set(number v) {
    this->gen_05_in6_set(v);
}

void numberobj_30_value_set(number v) {
    this->numberobj_30_value_setter(v);
    v = this->numberobj_30_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_30_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-85"), localvalue, this->_currentTime);
    this->numberobj_30_output_set(localvalue);
}

void gen_04_in6_set(number v) {
    this->gen_04_in6 = v;
}

void numberobj_16_output_set(number v) {
    this->gen_04_in6_set(v);
}

void numberobj_16_value_set(number v) {
    this->numberobj_16_value_setter(v);
    v = this->numberobj_16_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_16_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-72"), localvalue, this->_currentTime);
    this->numberobj_16_output_set(localvalue);
}

void gen_01_in6_set(number v) {
    this->gen_01_in6 = v;
}

void numberobj_04_output_set(number v) {
    this->gen_01_in6_set(v);
}

void numberobj_04_value_set(number v) {
    this->numberobj_04_value_setter(v);
    v = this->numberobj_04_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_04_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-61"), localvalue, this->_currentTime);
    this->numberobj_04_output_set(localvalue);
}

void numberobj_26_output_set(number v) {
    this->numberobj_30_value_set(v);
    this->numberobj_16_value_set(v);
    this->numberobj_04_value_set(v);
}

void numberobj_26_value_set(number v) {
    this->numberobj_26_value_setter(v);
    v = this->numberobj_26_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 1) {
        localvalue = 1;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_26_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-62"), localvalue, this->_currentTime);
    this->numberobj_26_output_set(localvalue);
}

number param_09_value_constrain(number v) const {
    v = (v > 127 ? 127 : (v < 0 ? 0 : v));
    return v;
}

void gen_05_in7_set(number v) {
    this->gen_05_in7 = v;
}

void numberobj_31_output_set(number v) {
    this->gen_05_in7_set(v);
}

void numberobj_31_value_set(number v) {
    this->numberobj_31_value_setter(v);
    v = this->numberobj_31_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_31_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-82"), localvalue, this->_currentTime);
    this->numberobj_31_output_set(localvalue);
}

void gen_04_in7_set(number v) {
    this->gen_04_in7 = v;
}

void numberobj_17_output_set(number v) {
    this->gen_04_in7_set(v);
}

void numberobj_17_value_set(number v) {
    this->numberobj_17_value_setter(v);
    v = this->numberobj_17_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_17_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-69"), localvalue, this->_currentTime);
    this->numberobj_17_output_set(localvalue);
}

void gen_01_in7_set(number v) {
    this->gen_01_in7 = v;
}

void numberobj_05_output_set(number v) {
    this->gen_01_in7_set(v);
}

void numberobj_05_value_set(number v) {
    this->numberobj_05_value_setter(v);
    v = this->numberobj_05_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_05_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-65"), localvalue, this->_currentTime);
    this->numberobj_05_output_set(localvalue);
}

void numberobj_34_output_set(number v) {
    this->numberobj_31_value_set(v);
    this->numberobj_17_value_set(v);
    this->numberobj_05_value_set(v);
}

void numberobj_34_value_set(number v) {
    this->numberobj_34_value_setter(v);
    v = this->numberobj_34_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 1) {
        localvalue = 1;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_34_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-99"), localvalue, this->_currentTime);
    this->numberobj_34_output_set(localvalue);
}

number param_10_value_constrain(number v) const {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void dspexpr_11_in2_set(number v) {
    this->dspexpr_11_in2 = v;
}

void numberobj_33_output_set(number v) {
    this->dspexpr_11_in2_set(v);
}

void numberobj_33_value_set(number v) {
    this->numberobj_33_value_setter(v);
    v = this->numberobj_33_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 2) {
        localvalue = 2;
    }

    if (this->numberobj_33_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-164"), localvalue, this->_currentTime);
    this->numberobj_33_output_set(localvalue);
}

number param_11_value_constrain(number v) const {
    v = (v > 127 ? 127 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_in2_set(number v) {
    this->gen_02_in2 = v;
}

void numberobj_37_output_set(number v) {
    this->gen_02_in2_set(v);
}

void numberobj_37_value_set(number v) {
    this->numberobj_37_value_setter(v);
    v = this->numberobj_37_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_37_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-26"), localvalue, this->_currentTime);
    this->numberobj_37_output_set(localvalue);
}

void gen_01_in8_set(number v) {
    this->gen_01_in8 = v;
}

void numberobj_06_output_set(number v) {
    this->gen_01_in8_set(v);
}

void numberobj_06_value_set(number v) {
    this->numberobj_06_value_setter(v);
    v = this->numberobj_06_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_06_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-64"), localvalue, this->_currentTime);
    this->numberobj_06_output_set(localvalue);
}

void loadmess_01_message_bang() {
    list v = this->loadmess_01_message;

    {
        number converted = (v->length > 0 ? v[0] : 0);
        this->numberobj_07_value_set(converted);
    }
}

void loadmess_02_message_bang() {
    list v = this->loadmess_02_message;

    {
        number converted = (v->length > 0 ? v[0] : 0);
        this->numberobj_08_value_set(converted);
    }
}

void loadmess_03_message_bang() {
    list v = this->loadmess_03_message;

    {
        number converted = (v->length > 0 ? v[0] : 0);
        this->numberobj_09_value_set(converted);
    }
}

void gen_03_in2_set(number v) {
    this->gen_03_in2 = v;
}

void numberobj_10_output_set(number v) {
    this->gen_03_in2_set(v);
}

void numberobj_10_value_set(number v) {
    this->numberobj_10_value_setter(v);
    v = this->numberobj_10_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_10_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-55"), localvalue, this->_currentTime);
    this->numberobj_10_output_set(localvalue);
}

void loadmess_04_message_bang() {
    list v = this->loadmess_04_message;

    {
        number converted = (v->length > 0 ? v[0] : 0);
        this->numberobj_10_value_set(converted);
    }
}

void dspexpr_08_in2_set(number v) {
    this->dspexpr_08_in2 = v;
}

void numberobj_11_output_set(number v) {
    this->dspexpr_08_in2_set(v);
}

void numberobj_11_value_set(number v) {
    this->numberobj_11_value_setter(v);
    v = this->numberobj_11_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 50) {
        localvalue = 50;
    }

    if (this->numberobj_11_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-16"), localvalue, this->_currentTime);
    this->numberobj_11_output_set(localvalue);
}

void gen_03_in3_set(number v) {
    this->gen_03_in3 = v;
}

void numberobj_14_output_set(number v) {
    this->gen_03_in3_set(v);
}

void numberobj_14_value_set(number v) {
    this->numberobj_14_value_setter(v);
    v = this->numberobj_14_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 1) {
        localvalue = 1;
    }

    if (this->numberobj_14_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-60"), localvalue, this->_currentTime);
    this->numberobj_14_output_set(localvalue);
}

void loadmess_05_message_bang() {
    list v = this->loadmess_05_message;

    {
        number converted = (v->length > 0 ? v[0] : 0);
        this->numberobj_11_value_set(converted);
    }
}

void gen_03_in4_set(number v) {
    this->gen_03_in4 = v;
}

void numberobj_18_output_set(number v) {
    this->gen_03_in4_set(v);
}

void numberobj_18_value_set(number v) {
    this->numberobj_18_value_setter(v);
    v = this->numberobj_18_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 2000) {
        localvalue = 2000;
    }

    if (this->numberobj_18_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-80"), localvalue, this->_currentTime);
    this->numberobj_18_output_set(localvalue);
}

void gen_04_in8_set(number v) {
    this->gen_04_in8 = v;
}

void numberobj_19_output_set(number v) {
    this->gen_04_in8_set(v);
}

void numberobj_19_value_set(number v) {
    this->numberobj_19_value_setter(v);
    v = this->numberobj_19_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_19_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-68"), localvalue, this->_currentTime);
    this->numberobj_19_output_set(localvalue);
}

void loadmess_06_message_bang() {
    list v = this->loadmess_06_message;

    {
        number converted = (v->length > 0 ? v[0] : 0);
        this->numberobj_14_value_set(converted);
    }
}

void gen_03_in5_set(number v) {
    this->gen_03_in5 = v;
}

void numberobj_21_output_set(number v) {
    this->gen_03_in5_set(v);
}

void numberobj_21_value_set(number v) {
    this->numberobj_21_value_setter(v);
    v = this->numberobj_21_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0.5) {
        localvalue = 0.5;
    }

    if ((bool)(true) && localvalue > 0.8) {
        localvalue = 0.8;
    }

    if (this->numberobj_21_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-97"), localvalue, this->_currentTime);
    this->numberobj_21_output_set(localvalue);
}

void loadmess_07_message_bang() {
    list v = this->loadmess_07_message;

    {
        number converted = (v->length > 0 ? v[0] : 0);
        this->numberobj_22_value_set(converted);
    }
}

void loadmess_08_message_bang() {
    list v = this->loadmess_08_message;

    {
        number converted = (v->length > 0 ? v[0] : 0);
        this->slider_01_input_set(converted);
    }
}

void gen_03_in6_set(number v) {
    this->gen_03_in6 = v;
}

void numberobj_23_output_set(number v) {
    this->gen_03_in6_set(v);
}

void numberobj_23_value_set(number v) {
    this->numberobj_23_value_setter(v);
    v = this->numberobj_23_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 1) {
        localvalue = 1;
    }

    if (this->numberobj_23_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-142"), localvalue, this->_currentTime);
    this->numberobj_23_output_set(localvalue);
}

void loadmess_09_message_bang() {
    list v = this->loadmess_09_message;

    {
        number converted = (v->length > 0 ? v[0] : 0);
        this->numberobj_18_value_set(converted);
    }
}

void loadmess_10_message_bang() {
    list v = this->loadmess_10_message;

    {
        number converted = (v->length > 0 ? v[0] : 0);
        this->numberobj_21_value_set(converted);
    }
}

void loadmess_11_message_bang() {
    list v = this->loadmess_11_message;

    {
        number converted = (v->length > 0 ? v[0] : 0);
        this->numberobj_24_value_set(converted);
    }
}

void loadmess_12_message_bang() {
    list v = this->loadmess_12_message;

    {
        number converted = (v->length > 0 ? v[0] : 0);
        this->numberobj_23_value_set(converted);
    }
}

void loadmess_13_message_bang() {
    list v = this->loadmess_13_message;

    {
        number converted = (v->length > 0 ? v[0] : 0);
        this->numberobj_25_value_set(converted);
    }
}

void loadmess_14_message_bang() {
    list v = this->loadmess_14_message;

    {
        number converted = (v->length > 0 ? v[0] : 0);
        this->numberobj_26_value_set(converted);
    }
}

void gen_05_in8_set(number v) {
    this->gen_05_in8 = v;
}

void numberobj_32_output_set(number v) {
    this->gen_05_in8_set(v);
}

void numberobj_32_value_set(number v) {
    this->numberobj_32_value_setter(v);
    v = this->numberobj_32_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_32_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-81"), localvalue, this->_currentTime);
    this->numberobj_32_output_set(localvalue);
}

void loadmess_15_message_bang() {
    list v = this->loadmess_15_message;

    {
        number converted = (v->length > 0 ? v[0] : 0);
        this->numberobj_33_value_set(converted);
    }
}

void loadmess_16_message_bang() {
    list v = this->loadmess_16_message;

    {
        number converted = (v->length > 0 ? v[0] : 0);
        this->numberobj_34_value_set(converted);
    }
}

void numberobj_35_output_set(number v) {
    this->numberobj_32_value_set(v);
    this->numberobj_19_value_set(v);
    this->numberobj_06_value_set(v);
}

void numberobj_35_value_set(number v) {
    this->numberobj_35_value_setter(v);
    v = this->numberobj_35_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 1) {
        localvalue = 1;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_35_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-104"), localvalue, this->_currentTime);
    this->numberobj_35_output_set(localvalue);
}

void loadmess_17_message_bang() {
    list v = this->loadmess_17_message;

    {
        number converted = (v->length > 0 ? v[0] : 0);
        this->numberobj_35_value_set(converted);
    }
}

void gen_02_in3_set(number v) {
    this->gen_02_in3 = v;
}

void numberobj_36_output_set(number v) {
    this->gen_02_in3_set(v);
}

void numberobj_36_value_set(number v) {
    this->numberobj_36_value_setter(v);
    v = this->numberobj_36_value;
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_36_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->getEngine()->sendNumMessage(TAG("valout"), TAG("number_obj-34"), localvalue, this->_currentTime);
    this->numberobj_36_output_set(localvalue);
}

void signaladder_01_perform(
    const SampleValue * in1,
    const SampleValue * in2,
    SampleValue * out,
    Index n
) {
    Index i;

    for (i = 0; i < n; i++) {
        out[(Index)i] = in1[(Index)i] + in2[(Index)i];
    }
}

void dspexpr_08_perform(const Sample * in1, number in2, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] * in2;//#map:_###_obj_###_:1
    }
}

void dspexpr_09_perform(const Sample * in1, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = rnbo_tanh(in1[(Index)i]);//#map:_###_obj_###_:1
    }
}

void signalgate_02_perform(
    number onoff,
    const SampleValue * input,
    SampleValue * out1,
    SampleValue * out2,
    SampleValue * out3,
    SampleValue * out4,
    SampleValue * out5,
    SampleValue * out6,
    SampleValue * out7,
    SampleValue * out8,
    SampleValue * out9,
    SampleValue * out10,
    SampleValue * out11,
    SampleValue * out12,
    Index n
) {
    Index i;

    for (i = 0; i < n; i++) {
        if (onoff == 1) {
            out1[(Index)i] = input[(Index)i];
            out2[(Index)i] = 0;
            out3[(Index)i] = 0;
            out4[(Index)i] = 0;
            out5[(Index)i] = 0;
            out6[(Index)i] = 0;
            out7[(Index)i] = 0;
            out8[(Index)i] = 0;
            out9[(Index)i] = 0;
            out10[(Index)i] = 0;
            out11[(Index)i] = 0;
            out12[(Index)i] = 0;
        } else if (onoff == 2) {
            out2[(Index)i] = input[(Index)i];
            out1[(Index)i] = 0;
            out3[(Index)i] = 0;
            out4[(Index)i] = 0;
            out5[(Index)i] = 0;
            out6[(Index)i] = 0;
            out7[(Index)i] = 0;
            out8[(Index)i] = 0;
            out9[(Index)i] = 0;
            out10[(Index)i] = 0;
            out11[(Index)i] = 0;
            out12[(Index)i] = 0;
        } else if (onoff == 3) {
            out3[(Index)i] = input[(Index)i];
            out1[(Index)i] = 0;
            out2[(Index)i] = 0;
            out4[(Index)i] = 0;
            out5[(Index)i] = 0;
            out6[(Index)i] = 0;
            out7[(Index)i] = 0;
            out8[(Index)i] = 0;
            out9[(Index)i] = 0;
            out10[(Index)i] = 0;
            out11[(Index)i] = 0;
            out12[(Index)i] = 0;
        } else if (onoff == 4) {
            out4[(Index)i] = input[(Index)i];
            out1[(Index)i] = 0;
            out2[(Index)i] = 0;
            out3[(Index)i] = 0;
            out5[(Index)i] = 0;
            out6[(Index)i] = 0;
            out7[(Index)i] = 0;
            out8[(Index)i] = 0;
            out9[(Index)i] = 0;
            out10[(Index)i] = 0;
            out11[(Index)i] = 0;
            out12[(Index)i] = 0;
        } else if (onoff == 5) {
            out5[(Index)i] = input[(Index)i];
            out1[(Index)i] = 0;
            out2[(Index)i] = 0;
            out3[(Index)i] = 0;
            out4[(Index)i] = 0;
            out6[(Index)i] = 0;
            out7[(Index)i] = 0;
            out8[(Index)i] = 0;
            out9[(Index)i] = 0;
            out10[(Index)i] = 0;
            out11[(Index)i] = 0;
            out12[(Index)i] = 0;
        } else if (onoff == 6) {
            out6[(Index)i] = input[(Index)i];
            out1[(Index)i] = 0;
            out2[(Index)i] = 0;
            out3[(Index)i] = 0;
            out4[(Index)i] = 0;
            out5[(Index)i] = 0;
            out7[(Index)i] = 0;
            out8[(Index)i] = 0;
            out9[(Index)i] = 0;
            out10[(Index)i] = 0;
            out11[(Index)i] = 0;
            out12[(Index)i] = 0;
        } else if (onoff == 7) {
            out7[(Index)i] = input[(Index)i];
            out1[(Index)i] = 0;
            out2[(Index)i] = 0;
            out3[(Index)i] = 0;
            out4[(Index)i] = 0;
            out5[(Index)i] = 0;
            out6[(Index)i] = 0;
            out8[(Index)i] = 0;
            out9[(Index)i] = 0;
            out10[(Index)i] = 0;
            out11[(Index)i] = 0;
            out12[(Index)i] = 0;
        } else if (onoff == 8) {
            out8[(Index)i] = input[(Index)i];
            out1[(Index)i] = 0;
            out2[(Index)i] = 0;
            out3[(Index)i] = 0;
            out4[(Index)i] = 0;
            out5[(Index)i] = 0;
            out6[(Index)i] = 0;
            out7[(Index)i] = 0;
            out9[(Index)i] = 0;
            out10[(Index)i] = 0;
            out11[(Index)i] = 0;
            out12[(Index)i] = 0;
        } else if (onoff == 9) {
            out9[(Index)i] = input[(Index)i];
            out1[(Index)i] = 0;
            out2[(Index)i] = 0;
            out3[(Index)i] = 0;
            out4[(Index)i] = 0;
            out5[(Index)i] = 0;
            out6[(Index)i] = 0;
            out7[(Index)i] = 0;
            out8[(Index)i] = 0;
            out10[(Index)i] = 0;
            out11[(Index)i] = 0;
            out12[(Index)i] = 0;
        } else if (onoff == 10) {
            out10[(Index)i] = input[(Index)i];
            out1[(Index)i] = 0;
            out2[(Index)i] = 0;
            out3[(Index)i] = 0;
            out4[(Index)i] = 0;
            out5[(Index)i] = 0;
            out6[(Index)i] = 0;
            out7[(Index)i] = 0;
            out8[(Index)i] = 0;
            out9[(Index)i] = 0;
            out11[(Index)i] = 0;
            out12[(Index)i] = 0;
        } else if (onoff == 11) {
            out11[(Index)i] = input[(Index)i];
            out1[(Index)i] = 0;
            out2[(Index)i] = 0;
            out3[(Index)i] = 0;
            out4[(Index)i] = 0;
            out5[(Index)i] = 0;
            out6[(Index)i] = 0;
            out7[(Index)i] = 0;
            out8[(Index)i] = 0;
            out9[(Index)i] = 0;
            out10[(Index)i] = 0;
            out12[(Index)i] = 0;
        } else if (onoff == 12) {
            out12[(Index)i] = input[(Index)i];
            out1[(Index)i] = 0;
            out2[(Index)i] = 0;
            out3[(Index)i] = 0;
            out4[(Index)i] = 0;
            out5[(Index)i] = 0;
            out6[(Index)i] = 0;
            out7[(Index)i] = 0;
            out8[(Index)i] = 0;
            out9[(Index)i] = 0;
            out10[(Index)i] = 0;
            out11[(Index)i] = 0;
        } else {
            out1[(Index)i] = 0;
            out2[(Index)i] = 0;
            out3[(Index)i] = 0;
            out4[(Index)i] = 0;
            out5[(Index)i] = 0;
            out6[(Index)i] = 0;
            out7[(Index)i] = 0;
            out8[(Index)i] = 0;
            out9[(Index)i] = 0;
            out10[(Index)i] = 0;
            out11[(Index)i] = 0;
            out12[(Index)i] = 0;
        }
    }
}

void signaladder_02_perform(
    const SampleValue * in1,
    const SampleValue * in2,
    const SampleValue * in3,
    const SampleValue * in4,
    const SampleValue * in5,
    SampleValue * out,
    Index n
) {
    Index i;

    for (i = 0; i < n; i++) {
        out[(Index)i] = in1[(Index)i] + in2[(Index)i] + in3[(Index)i] + in4[(Index)i] + in5[(Index)i];
    }
}

void signaladder_03_perform(
    const SampleValue * in1,
    const SampleValue * in2,
    const SampleValue * in3,
    const SampleValue * in4,
    const SampleValue * in5,
    SampleValue * out,
    Index n
) {
    Index i;

    for (i = 0; i < n; i++) {
        out[(Index)i] = in1[(Index)i] + in2[(Index)i] + in3[(Index)i] + in4[(Index)i] + in5[(Index)i];
    }
}

void gen_01_perform(
    const Sample * in1,
    const Sample * in2,
    number in3,
    number in4,
    number in5,
    number in6,
    number in7,
    number in8,
    SampleValue * out1,
    SampleValue * out2,
    Index n
) {
    auto __gen_01_history_10_value = this->gen_01_history_10_value;
    auto __gen_01_history_9_value = this->gen_01_history_9_value;
    auto __gen_01_history_12_value = this->gen_01_history_12_value;
    auto __gen_01_history_8_value = this->gen_01_history_8_value;
    auto __gen_01_history_7_value = this->gen_01_history_7_value;
    auto __gen_01_history_11_value = this->gen_01_history_11_value;
    auto __gen_01_history_6_value = this->gen_01_history_6_value;
    auto __gen_01_history_5_value = this->gen_01_history_5_value;
    auto __gen_01_history_2_value = this->gen_01_history_2_value;
    auto __gen_01_history_1_value = this->gen_01_history_1_value;
    number div_13_0 = in7 / (number)127;
    number div_15_2 = in8 / (number)127;
    auto mstosamps_21_8 = this->mstosamps(in3);
    auto mstosamps_23_10 = this->mstosamps(in3);
    number div_25_12 = in7 / (number)127;
    number div_27_14 = in8 / (number)127;
    number div_34_21 = in6 / (number)127;
    number div_35_22 = in5 / (number)127;
    number div_43_30 = in5 / (number)127;
    number div_45_32 = in6 / (number)127;
    Index i;

    for (i = 0; i < n; i++) {
        number mix_14_1 = in1[(Index)i] + div_13_0 * (__gen_01_history_1_value - in1[(Index)i]);
        number mix_16_3 = mix_14_1 + div_15_2 * (__gen_01_history_2_value - mix_14_1);
        number sub_17_4 = mix_16_3 - 0;
        number gen_18_5 = sub_17_4;
        number history_1_next_19_6 = fixdenorm(mix_14_1);
        number history_2_next_20_7 = fixdenorm(mix_16_3);
        number tap_22_9 = this->gen_01_delay_3_read(mstosamps_21_8, 0);
        out1[(Index)i] = tap_22_9;
        number tap_24_11 = this->gen_01_delay_4_read(mstosamps_23_10, 0);
        out2[(Index)i] = tap_24_11;
        number mix_26_13 = in2[(Index)i] + div_25_12 * (__gen_01_history_5_value - in2[(Index)i]);
        number mix_28_15 = mix_26_13 + div_27_14 * (__gen_01_history_6_value - mix_26_13);
        number sub_29_16 = mix_28_15 - 0;
        number gen_30_17 = sub_29_16;
        number history_5_next_31_18 = fixdenorm(mix_26_13);
        number history_6_next_32_19 = fixdenorm(mix_28_15);
        number mul_33_20 = __gen_01_history_11_value * in4;
        number mix_36_23 = mul_33_20 + div_35_22 * (__gen_01_history_7_value - mul_33_20);
        number mix_37_24 = mix_36_23 + div_34_21 * (__gen_01_history_8_value - mix_36_23);
        number sub_38_25 = mix_37_24 - 0;
        number gen_39_26 = sub_38_25;
        number history_7_next_40_27 = fixdenorm(mix_36_23);
        number history_8_next_41_28 = fixdenorm(mix_37_24);
        number mul_42_29 = __gen_01_history_12_value * in4;
        number mix_44_31 = mul_42_29 + div_43_30 * (__gen_01_history_9_value - mul_42_29);
        number mix_46_33 = mix_44_31 + div_45_32 * (__gen_01_history_10_value - mix_44_31);
        number sub_47_34 = mix_46_33 - 0;
        number gen_48_35 = sub_47_34;
        number history_9_next_49_36 = fixdenorm(mix_44_31);
        number history_10_next_50_37 = fixdenorm(mix_46_33);
        number history_11_next_51_38 = fixdenorm(tap_22_9);
        number history_12_next_52_39 = fixdenorm(tap_24_11);
        __gen_01_history_1_value = history_1_next_19_6;
        __gen_01_history_12_value = history_12_next_52_39;
        __gen_01_history_11_value = history_11_next_51_38;
        __gen_01_history_10_value = history_10_next_50_37;
        __gen_01_history_9_value = history_9_next_49_36;
        __gen_01_history_8_value = history_8_next_41_28;
        __gen_01_history_7_value = history_7_next_40_27;
        __gen_01_history_6_value = history_6_next_32_19;
        __gen_01_history_5_value = history_5_next_31_18;
        this->gen_01_delay_4_write(gen_30_17 + gen_48_35);
        this->gen_01_delay_3_write(gen_18_5 + gen_39_26);
        __gen_01_history_2_value = history_2_next_20_7;
        this->gen_01_delay_3_step();
        this->gen_01_delay_4_step();
    }

    this->gen_01_history_1_value = __gen_01_history_1_value;
    this->gen_01_history_2_value = __gen_01_history_2_value;
    this->gen_01_history_5_value = __gen_01_history_5_value;
    this->gen_01_history_6_value = __gen_01_history_6_value;
    this->gen_01_history_11_value = __gen_01_history_11_value;
    this->gen_01_history_7_value = __gen_01_history_7_value;
    this->gen_01_history_8_value = __gen_01_history_8_value;
    this->gen_01_history_12_value = __gen_01_history_12_value;
    this->gen_01_history_9_value = __gen_01_history_9_value;
    this->gen_01_history_10_value = __gen_01_history_10_value;
}

void signaladder_04_perform(
    const SampleValue * in1,
    const SampleValue * in2,
    SampleValue * out,
    Index n
) {
    Index i;

    for (i = 0; i < n; i++) {
        out[(Index)i] = in1[(Index)i] + in2[(Index)i];
    }
}

void dspexpr_03_perform(const Sample * in1, number in2, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] * in2;//#map:_###_obj_###_:1
    }
}

void dspexpr_02_perform(const Sample * in1, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = rnbo_tanh(in1[(Index)i]);//#map:_###_obj_###_:1
    }
}

void signaladder_05_perform(
    const SampleValue * in1,
    const SampleValue * in2,
    const SampleValue * in3,
    const SampleValue * in4,
    const SampleValue * in5,
    const SampleValue * in6,
    SampleValue * out,
    Index n
) {
    Index i;

    for (i = 0; i < n; i++) {
        out[(Index)i] = in1[(Index)i] + in2[(Index)i] + in3[(Index)i] + in4[(Index)i] + in5[(Index)i] + in6[(Index)i];
    }
}

void signaladder_06_perform(
    const SampleValue * in1,
    const SampleValue * in2,
    const SampleValue * in3,
    const SampleValue * in4,
    const SampleValue * in5,
    const SampleValue * in6,
    SampleValue * out,
    Index n
) {
    Index i;

    for (i = 0; i < n; i++) {
        out[(Index)i] = in1[(Index)i] + in2[(Index)i] + in3[(Index)i] + in4[(Index)i] + in5[(Index)i] + in6[(Index)i];
    }
}

void gen_04_perform(
    const Sample * in1,
    const Sample * in2,
    number in3,
    number in4,
    number in5,
    number in6,
    number in7,
    number in8,
    SampleValue * out1,
    SampleValue * out2,
    Index n
) {
    auto __gen_04_history_10_value = this->gen_04_history_10_value;
    auto __gen_04_history_9_value = this->gen_04_history_9_value;
    auto __gen_04_history_12_value = this->gen_04_history_12_value;
    auto __gen_04_history_8_value = this->gen_04_history_8_value;
    auto __gen_04_history_7_value = this->gen_04_history_7_value;
    auto __gen_04_history_11_value = this->gen_04_history_11_value;
    auto __gen_04_history_6_value = this->gen_04_history_6_value;
    auto __gen_04_history_5_value = this->gen_04_history_5_value;
    auto __gen_04_history_2_value = this->gen_04_history_2_value;
    auto __gen_04_history_1_value = this->gen_04_history_1_value;
    number div_13_0 = in7 / (number)127;
    number div_15_2 = in8 / (number)127;
    auto mstosamps_21_8 = this->mstosamps(in3);
    auto mstosamps_23_10 = this->mstosamps(in3);
    number div_25_12 = in7 / (number)127;
    number div_27_14 = in8 / (number)127;
    number div_34_21 = in5 / (number)127;
    number div_36_23 = in6 / (number)127;
    number div_43_30 = in5 / (number)127;
    number div_45_32 = in6 / (number)127;
    Index i;

    for (i = 0; i < n; i++) {
        number mix_14_1 = in2[(Index)i] + div_13_0 * (__gen_04_history_1_value - in2[(Index)i]);
        number mix_16_3 = mix_14_1 + div_15_2 * (__gen_04_history_2_value - mix_14_1);
        number sub_17_4 = mix_16_3 - 0;
        number gen_18_5 = sub_17_4;
        number history_1_next_19_6 = fixdenorm(mix_14_1);
        number history_2_next_20_7 = fixdenorm(mix_16_3);
        number tap_22_9 = this->gen_04_delay_3_read(mstosamps_21_8, 0);
        out2[(Index)i] = tap_22_9;
        number tap_24_11 = this->gen_04_delay_4_read(mstosamps_23_10, 0);
        out1[(Index)i] = tap_24_11;
        number mix_26_13 = in1[(Index)i] + div_25_12 * (__gen_04_history_5_value - in1[(Index)i]);
        number mix_28_15 = mix_26_13 + div_27_14 * (__gen_04_history_6_value - mix_26_13);
        number sub_29_16 = mix_28_15 - 0;
        number gen_30_17 = sub_29_16;
        number history_5_next_31_18 = fixdenorm(mix_26_13);
        number history_6_next_32_19 = fixdenorm(mix_28_15);
        number mul_33_20 = __gen_04_history_11_value * in4;
        number mix_35_22 = mul_33_20 + div_34_21 * (__gen_04_history_7_value - mul_33_20);
        number mix_37_24 = mix_35_22 + div_36_23 * (__gen_04_history_8_value - mix_35_22);
        number sub_38_25 = mix_37_24 - 0;
        number gen_39_26 = sub_38_25;
        number history_7_next_40_27 = fixdenorm(mix_35_22);
        number history_8_next_41_28 = fixdenorm(mix_37_24);
        number mul_42_29 = __gen_04_history_12_value * in4;
        number mix_44_31 = mul_42_29 + div_43_30 * (__gen_04_history_9_value - mul_42_29);
        number mix_46_33 = mix_44_31 + div_45_32 * (__gen_04_history_10_value - mix_44_31);
        number sub_47_34 = mix_46_33 - 0;
        number gen_48_35 = sub_47_34;
        number history_9_next_49_36 = fixdenorm(mix_44_31);
        number history_10_next_50_37 = fixdenorm(mix_46_33);
        number history_11_next_51_38 = fixdenorm(tap_22_9);
        number history_12_next_52_39 = fixdenorm(tap_24_11);
        __gen_04_history_1_value = history_1_next_19_6;
        __gen_04_history_12_value = history_12_next_52_39;
        __gen_04_history_11_value = history_11_next_51_38;
        __gen_04_history_10_value = history_10_next_50_37;
        __gen_04_history_9_value = history_9_next_49_36;
        __gen_04_history_8_value = history_8_next_41_28;
        __gen_04_history_7_value = history_7_next_40_27;
        __gen_04_history_6_value = history_6_next_32_19;
        __gen_04_history_5_value = history_5_next_31_18;
        this->gen_04_delay_4_write(gen_30_17 + gen_48_35);
        this->gen_04_delay_3_write(gen_18_5 + gen_39_26);
        __gen_04_history_2_value = history_2_next_20_7;
        this->gen_04_delay_3_step();
        this->gen_04_delay_4_step();
    }

    this->gen_04_history_1_value = __gen_04_history_1_value;
    this->gen_04_history_2_value = __gen_04_history_2_value;
    this->gen_04_history_5_value = __gen_04_history_5_value;
    this->gen_04_history_6_value = __gen_04_history_6_value;
    this->gen_04_history_11_value = __gen_04_history_11_value;
    this->gen_04_history_7_value = __gen_04_history_7_value;
    this->gen_04_history_8_value = __gen_04_history_8_value;
    this->gen_04_history_12_value = __gen_04_history_12_value;
    this->gen_04_history_9_value = __gen_04_history_9_value;
    this->gen_04_history_10_value = __gen_04_history_10_value;
}

void signaladder_07_perform(
    const SampleValue * in1,
    const SampleValue * in2,
    SampleValue * out,
    Index n
) {
    Index i;

    for (i = 0; i < n; i++) {
        out[(Index)i] = in1[(Index)i] + in2[(Index)i];
    }
}

void dspexpr_07_perform(const Sample * in1, number in2, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] * in2;//#map:_###_obj_###_:1
    }
}

void dspexpr_06_perform(const Sample * in1, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = rnbo_tanh(in1[(Index)i]);//#map:_###_obj_###_:1
    }
}

void signaladder_08_perform(
    const SampleValue * in1,
    const SampleValue * in2,
    const SampleValue * in3,
    const SampleValue * in4,
    const SampleValue * in5,
    const SampleValue * in6,
    SampleValue * out,
    Index n
) {
    Index i;

    for (i = 0; i < n; i++) {
        out[(Index)i] = in1[(Index)i] + in2[(Index)i] + in3[(Index)i] + in4[(Index)i] + in5[(Index)i] + in6[(Index)i];
    }
}

void signaladder_09_perform(
    const SampleValue * in1,
    const SampleValue * in2,
    const SampleValue * in3,
    const SampleValue * in4,
    const SampleValue * in5,
    const SampleValue * in6,
    SampleValue * out,
    Index n
) {
    Index i;

    for (i = 0; i < n; i++) {
        out[(Index)i] = in1[(Index)i] + in2[(Index)i] + in3[(Index)i] + in4[(Index)i] + in5[(Index)i] + in6[(Index)i];
    }
}

void gen_05_perform(
    const Sample * in1,
    const Sample * in2,
    number in3,
    number in4,
    number in5,
    number in6,
    number in7,
    number in8,
    SampleValue * out1,
    SampleValue * out2,
    Index n
) {
    auto __gen_05_history_10_value = this->gen_05_history_10_value;
    auto __gen_05_history_9_value = this->gen_05_history_9_value;
    auto __gen_05_history_12_value = this->gen_05_history_12_value;
    auto __gen_05_history_8_value = this->gen_05_history_8_value;
    auto __gen_05_history_7_value = this->gen_05_history_7_value;
    auto __gen_05_history_11_value = this->gen_05_history_11_value;
    auto __gen_05_history_6_value = this->gen_05_history_6_value;
    auto __gen_05_history_5_value = this->gen_05_history_5_value;
    auto __gen_05_history_2_value = this->gen_05_history_2_value;
    auto __gen_05_history_1_value = this->gen_05_history_1_value;
    number div_13_0 = in8 / (number)127;
    number div_14_1 = in7 / (number)127;
    auto mstosamps_21_8 = this->mstosamps(in3);
    auto mstosamps_23_10 = this->mstosamps(in3);
    number div_25_12 = in7 / (number)127;
    number div_27_14 = in8 / (number)127;
    number div_34_21 = in6 / (number)127;
    number div_35_22 = in5 / (number)127;
    number div_43_30 = in6 / (number)127;
    number div_44_31 = in5 / (number)127;
    Index i;

    for (i = 0; i < n; i++) {
        number mix_15_2 = in1[(Index)i] + div_14_1 * (__gen_05_history_1_value - in1[(Index)i]);
        number mix_16_3 = mix_15_2 + div_13_0 * (__gen_05_history_2_value - mix_15_2);
        number sub_17_4 = mix_16_3 - 0;
        number gen_18_5 = sub_17_4;
        number history_1_next_19_6 = fixdenorm(mix_15_2);
        number history_2_next_20_7 = fixdenorm(mix_16_3);
        number tap_22_9 = this->gen_05_delay_3_read(mstosamps_21_8, 0);
        out1[(Index)i] = tap_22_9;
        number tap_24_11 = this->gen_05_delay_4_read(mstosamps_23_10, 0);
        out2[(Index)i] = tap_24_11;
        number mix_26_13 = in2[(Index)i] + div_25_12 * (__gen_05_history_5_value - in2[(Index)i]);
        number mix_28_15 = mix_26_13 + div_27_14 * (__gen_05_history_6_value - mix_26_13);
        number sub_29_16 = mix_28_15 - 0;
        number gen_30_17 = sub_29_16;
        number history_5_next_31_18 = fixdenorm(mix_26_13);
        number history_6_next_32_19 = fixdenorm(mix_28_15);
        number mul_33_20 = __gen_05_history_11_value * in4;
        number mix_36_23 = mul_33_20 + div_35_22 * (__gen_05_history_7_value - mul_33_20);
        number mix_37_24 = mix_36_23 + div_34_21 * (__gen_05_history_8_value - mix_36_23);
        number sub_38_25 = mix_37_24 - 0;
        number gen_39_26 = sub_38_25;
        number history_7_next_40_27 = fixdenorm(mix_36_23);
        number history_8_next_41_28 = fixdenorm(mix_37_24);
        number mul_42_29 = __gen_05_history_12_value * in4;
        number mix_45_32 = mul_42_29 + div_44_31 * (__gen_05_history_9_value - mul_42_29);
        number mix_46_33 = mix_45_32 + div_43_30 * (__gen_05_history_10_value - mix_45_32);
        number sub_47_34 = mix_46_33 - 0;
        number gen_48_35 = sub_47_34;
        number history_9_next_49_36 = fixdenorm(mix_45_32);
        number history_10_next_50_37 = fixdenorm(mix_46_33);
        number history_11_next_51_38 = fixdenorm(tap_22_9);
        number history_12_next_52_39 = fixdenorm(tap_24_11);
        __gen_05_history_1_value = history_1_next_19_6;
        __gen_05_history_12_value = history_12_next_52_39;
        __gen_05_history_11_value = history_11_next_51_38;
        __gen_05_history_10_value = history_10_next_50_37;
        __gen_05_history_9_value = history_9_next_49_36;
        __gen_05_history_8_value = history_8_next_41_28;
        __gen_05_history_7_value = history_7_next_40_27;
        __gen_05_history_6_value = history_6_next_32_19;
        __gen_05_history_5_value = history_5_next_31_18;
        this->gen_05_delay_4_write(gen_30_17 + gen_48_35);
        this->gen_05_delay_3_write(gen_18_5 + gen_39_26);
        __gen_05_history_2_value = history_2_next_20_7;
        this->gen_05_delay_3_step();
        this->gen_05_delay_4_step();
    }

    this->gen_05_history_1_value = __gen_05_history_1_value;
    this->gen_05_history_2_value = __gen_05_history_2_value;
    this->gen_05_history_5_value = __gen_05_history_5_value;
    this->gen_05_history_6_value = __gen_05_history_6_value;
    this->gen_05_history_11_value = __gen_05_history_11_value;
    this->gen_05_history_7_value = __gen_05_history_7_value;
    this->gen_05_history_8_value = __gen_05_history_8_value;
    this->gen_05_history_12_value = __gen_05_history_12_value;
    this->gen_05_history_9_value = __gen_05_history_9_value;
    this->gen_05_history_10_value = __gen_05_history_10_value;
}

void signaladder_10_perform(
    const SampleValue * in1,
    const SampleValue * in2,
    SampleValue * out,
    Index n
) {
    Index i;

    for (i = 0; i < n; i++) {
        out[(Index)i] = in1[(Index)i] + in2[(Index)i];
    }
}

void dspexpr_11_perform(const Sample * in1, number in2, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] * in2;//#map:_###_obj_###_:1
    }
}

void dspexpr_10_perform(const Sample * in1, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = rnbo_tanh(in1[(Index)i]);//#map:_###_obj_###_:1
    }
}

void dspexpr_01_perform(const Sample * in1, number in2, SampleValue * out1, Index n) {
    RNBO_UNUSED(in2);
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] * 1;//#map:_###_obj_###_:1
    }
}

void signaladder_11_perform(
    const SampleValue * in1,
    const SampleValue * in2,
    const SampleValue * in3,
    const SampleValue * in4,
    SampleValue * out,
    Index n
) {
    Index i;

    for (i = 0; i < n; i++) {
        out[(Index)i] = in1[(Index)i] + in2[(Index)i] + in3[(Index)i] + in4[(Index)i];
    }
}

void signalgate_01_perform(
    number onoff,
    const SampleValue * input,
    SampleValue * out1,
    SampleValue * out2,
    SampleValue * out3,
    SampleValue * out4,
    SampleValue * out5,
    SampleValue * out6,
    SampleValue * out7,
    SampleValue * out8,
    SampleValue * out9,
    SampleValue * out10,
    SampleValue * out11,
    SampleValue * out12,
    Index n
) {
    Index i;

    for (i = 0; i < n; i++) {
        if (onoff == 1) {
            out1[(Index)i] = input[(Index)i];
            out2[(Index)i] = 0;
            out3[(Index)i] = 0;
            out4[(Index)i] = 0;
            out5[(Index)i] = 0;
            out6[(Index)i] = 0;
            out7[(Index)i] = 0;
            out8[(Index)i] = 0;
            out9[(Index)i] = 0;
            out10[(Index)i] = 0;
            out11[(Index)i] = 0;
            out12[(Index)i] = 0;
        } else if (onoff == 2) {
            out2[(Index)i] = input[(Index)i];
            out1[(Index)i] = 0;
            out3[(Index)i] = 0;
            out4[(Index)i] = 0;
            out5[(Index)i] = 0;
            out6[(Index)i] = 0;
            out7[(Index)i] = 0;
            out8[(Index)i] = 0;
            out9[(Index)i] = 0;
            out10[(Index)i] = 0;
            out11[(Index)i] = 0;
            out12[(Index)i] = 0;
        } else if (onoff == 3) {
            out3[(Index)i] = input[(Index)i];
            out1[(Index)i] = 0;
            out2[(Index)i] = 0;
            out4[(Index)i] = 0;
            out5[(Index)i] = 0;
            out6[(Index)i] = 0;
            out7[(Index)i] = 0;
            out8[(Index)i] = 0;
            out9[(Index)i] = 0;
            out10[(Index)i] = 0;
            out11[(Index)i] = 0;
            out12[(Index)i] = 0;
        } else if (onoff == 4) {
            out4[(Index)i] = input[(Index)i];
            out1[(Index)i] = 0;
            out2[(Index)i] = 0;
            out3[(Index)i] = 0;
            out5[(Index)i] = 0;
            out6[(Index)i] = 0;
            out7[(Index)i] = 0;
            out8[(Index)i] = 0;
            out9[(Index)i] = 0;
            out10[(Index)i] = 0;
            out11[(Index)i] = 0;
            out12[(Index)i] = 0;
        } else if (onoff == 5) {
            out5[(Index)i] = input[(Index)i];
            out1[(Index)i] = 0;
            out2[(Index)i] = 0;
            out3[(Index)i] = 0;
            out4[(Index)i] = 0;
            out6[(Index)i] = 0;
            out7[(Index)i] = 0;
            out8[(Index)i] = 0;
            out9[(Index)i] = 0;
            out10[(Index)i] = 0;
            out11[(Index)i] = 0;
            out12[(Index)i] = 0;
        } else if (onoff == 6) {
            out6[(Index)i] = input[(Index)i];
            out1[(Index)i] = 0;
            out2[(Index)i] = 0;
            out3[(Index)i] = 0;
            out4[(Index)i] = 0;
            out5[(Index)i] = 0;
            out7[(Index)i] = 0;
            out8[(Index)i] = 0;
            out9[(Index)i] = 0;
            out10[(Index)i] = 0;
            out11[(Index)i] = 0;
            out12[(Index)i] = 0;
        } else if (onoff == 7) {
            out7[(Index)i] = input[(Index)i];
            out1[(Index)i] = 0;
            out2[(Index)i] = 0;
            out3[(Index)i] = 0;
            out4[(Index)i] = 0;
            out5[(Index)i] = 0;
            out6[(Index)i] = 0;
            out8[(Index)i] = 0;
            out9[(Index)i] = 0;
            out10[(Index)i] = 0;
            out11[(Index)i] = 0;
            out12[(Index)i] = 0;
        } else if (onoff == 8) {
            out8[(Index)i] = input[(Index)i];
            out1[(Index)i] = 0;
            out2[(Index)i] = 0;
            out3[(Index)i] = 0;
            out4[(Index)i] = 0;
            out5[(Index)i] = 0;
            out6[(Index)i] = 0;
            out7[(Index)i] = 0;
            out9[(Index)i] = 0;
            out10[(Index)i] = 0;
            out11[(Index)i] = 0;
            out12[(Index)i] = 0;
        } else if (onoff == 9) {
            out9[(Index)i] = input[(Index)i];
            out1[(Index)i] = 0;
            out2[(Index)i] = 0;
            out3[(Index)i] = 0;
            out4[(Index)i] = 0;
            out5[(Index)i] = 0;
            out6[(Index)i] = 0;
            out7[(Index)i] = 0;
            out8[(Index)i] = 0;
            out10[(Index)i] = 0;
            out11[(Index)i] = 0;
            out12[(Index)i] = 0;
        } else if (onoff == 10) {
            out10[(Index)i] = input[(Index)i];
            out1[(Index)i] = 0;
            out2[(Index)i] = 0;
            out3[(Index)i] = 0;
            out4[(Index)i] = 0;
            out5[(Index)i] = 0;
            out6[(Index)i] = 0;
            out7[(Index)i] = 0;
            out8[(Index)i] = 0;
            out9[(Index)i] = 0;
            out11[(Index)i] = 0;
            out12[(Index)i] = 0;
        } else if (onoff == 11) {
            out11[(Index)i] = input[(Index)i];
            out1[(Index)i] = 0;
            out2[(Index)i] = 0;
            out3[(Index)i] = 0;
            out4[(Index)i] = 0;
            out5[(Index)i] = 0;
            out6[(Index)i] = 0;
            out7[(Index)i] = 0;
            out8[(Index)i] = 0;
            out9[(Index)i] = 0;
            out10[(Index)i] = 0;
            out12[(Index)i] = 0;
        } else if (onoff == 12) {
            out12[(Index)i] = input[(Index)i];
            out1[(Index)i] = 0;
            out2[(Index)i] = 0;
            out3[(Index)i] = 0;
            out4[(Index)i] = 0;
            out5[(Index)i] = 0;
            out6[(Index)i] = 0;
            out7[(Index)i] = 0;
            out8[(Index)i] = 0;
            out9[(Index)i] = 0;
            out10[(Index)i] = 0;
            out11[(Index)i] = 0;
        } else {
            out1[(Index)i] = 0;
            out2[(Index)i] = 0;
            out3[(Index)i] = 0;
            out4[(Index)i] = 0;
            out5[(Index)i] = 0;
            out6[(Index)i] = 0;
            out7[(Index)i] = 0;
            out8[(Index)i] = 0;
            out9[(Index)i] = 0;
            out10[(Index)i] = 0;
            out11[(Index)i] = 0;
            out12[(Index)i] = 0;
        }
    }
}

void signaladder_12_perform(
    const SampleValue * in1,
    const SampleValue * in2,
    const SampleValue * in3,
    const SampleValue * in4,
    const SampleValue * in5,
    const SampleValue * in6,
    const SampleValue * in7,
    const SampleValue * in8,
    SampleValue * out,
    Index n
) {
    Index i;

    for (i = 0; i < n; i++) {
        out[(Index)i] = in1[(Index)i] + in2[(Index)i] + in3[(Index)i] + in4[(Index)i] + in5[(Index)i] + in6[(Index)i] + in7[(Index)i] + in8[(Index)i];
    }
}

void gen_03_perform(
    const Sample * in1,
    number in2,
    number in3,
    number in4,
    number in5,
    number in6,
    SampleValue * out1,
    SampleValue * out2,
    Index n
) {
    auto __gen_03_history_31_value = this->gen_03_history_31_value;
    auto __gen_03_history_29_value = this->gen_03_history_29_value;
    auto __gen_03_history_27_value = this->gen_03_history_27_value;
    auto __gen_03_history_25_value = this->gen_03_history_25_value;
    auto __gen_03_history_22_value = this->gen_03_history_22_value;
    auto __gen_03_history_33_value = this->gen_03_history_33_value;
    auto __gen_03_history_19_value = this->gen_03_history_19_value;
    auto __gen_03_history_32_value = this->gen_03_history_32_value;
    number mul_37_3 = in6 * 9;
    number mul_39_5 = in6 * 99;
    number mul_41_7 = in6 * 64;
    number mul_43_9 = in6 * 67;
    number mul_45_11 = in6 * 67;
    number mul_47_13 = in6 * 60;
    number mul_49_15 = in6 * 67;
    number mul_51_17 = in6 * 90;
    number mul_53_19 = in6 * 41;
    number mul_55_21 = in6 * 11;
    number mul_57_23 = in6 * 71;
    number mul_59_25 = in6 * 122;
    number mul_61_27 = in6 * 12;
    auto int_63_29 = this->intnum(89);
    auto int_64_30 = this->intnum(60);
    number div_65_31 = in2 / (number)127;
    number div_66_32 = in2 / (number)127;
    auto int_68_34 = this->intnum(13);
    number mul_80_46 = in5 * -1;
    auto mstosamps_82_48 = this->mstosamps(int_63_29);
    number mul_95_61 = in5 * -1;
    auto mstosamps_97_63 = this->mstosamps(int_64_30);
    number mul_106_72 = in5 * -1;
    number mul_114_80 = in5 * -1;
    number mul_122_88 = in5 * -1;
    auto mstosamps_124_90 = this->mstosamps(int_68_34);
    number mul_130_96 = in5 * -1;
    Index i;

    for (i = 0; i < n; i++) {
        number tap_34_0 = this->gen_03_delay_1_read(8, 0);
        number tap_35_1 = this->gen_03_delay_2_read(106, 0);
        number tap_36_2 = this->gen_03_delay_3_read(125, 0);
        number tap_38_4 = this->gen_03_delay_4_read(mul_37_3, 0);
        number tap_40_6 = this->gen_03_delay_5_read(mul_39_5, 0);
        number tap_42_8 = this->gen_03_delay_6_read(mul_41_7, 0);
        number tap_44_10 = this->gen_03_delay_7_read(mul_43_9, 0);
        number tap_46_12 = this->gen_03_delay_8_read(mul_45_11, 0);
        number tap_48_14 = this->gen_03_delay_9_read(mul_47_13, 0);
        number tap_50_16 = this->gen_03_delay_10_read(mul_49_15, 0);
        out1[(Index)i] = tap_50_16 + tap_48_14 + tap_46_12 + tap_44_10 + tap_42_8 + tap_40_6 + tap_38_4;
        number tap_52_18 = this->gen_03_delay_11_read(mul_51_17, 0);
        number tap_54_20 = this->gen_03_delay_12_read(mul_53_19, 0);
        number tap_56_22 = this->gen_03_delay_13_read(mul_55_21, 0);
        number tap_58_24 = this->gen_03_delay_14_read(mul_57_23, 0);
        number tap_60_26 = this->gen_03_delay_15_read(mul_59_25, 0);
        number tap_62_28 = this->gen_03_delay_16_read(mul_61_27, 0);
        out2[(Index)i] = tap_62_28 + tap_60_26 + tap_58_24 + tap_56_22 + tap_54_20 + tap_52_18 + tap_34_0;
        number float_67_33 = float(9.3);
        number mul_69_35 = tap_36_2 * in3;
        number tanh_70_36 = rnbo_tanh(mul_69_35);
        number mul_71_37 = tap_35_1 * in3;
        number tanh_72_38 = rnbo_tanh(mul_71_37);
        number cycle_73 = 0;
        number cycleindex_74 = 0;
        array<number, 2> result_40 = this->gen_03_cycle_39_next(0.1, 0);
        cycleindex_74 = result_40[1];
        cycle_73 = result_40[0];
        number add_75_41 = cycle_73 + 1;
        number mul_76_42 = (add_75_41 + in6) * 31;
        auto mstosamps_77_43 = this->mstosamps(mul_76_42);
        number tap_78_44 = this->gen_03_delay_17_read(mstosamps_77_43, 0);
        number mix_79_45 = tap_78_44 + div_66_32 * (__gen_03_history_32_value - tap_78_44);
        number mul_81_47 = mul_80_46 * __gen_03_history_19_value;
        number tap_83_49 = this->gen_03_delay_18_read(mstosamps_82_48, 0);
        number mul_84_50 = tap_83_49 * in5;
        number add_85_51 = mul_84_50 + mul_81_47;
        number gen_86_52 = add_85_51;
        number history_19_next_87_53 = fixdenorm(add_85_51);
        number cycle_88 = 0;
        number cycleindex_89 = 0;
        array<number, 2> result_55 = this->gen_03_cycle_54_next(0.1, 0);
        cycleindex_89 = result_55[1];
        cycle_88 = result_55[0];
        number add_90_56 = cycle_88 + 1;
        number mul_91_57 = (add_90_56 + in6) * 23;
        auto mstosamps_92_58 = this->mstosamps(mul_91_57);
        number tap_93_59 = this->gen_03_delay_20_read(mstosamps_92_58, 0);
        number mix_94_60 = tap_93_59 + div_65_31 * (__gen_03_history_33_value - tap_93_59);
        number mul_96_62 = mul_95_61 * __gen_03_history_22_value;
        number tap_98_64 = this->gen_03_delay_21_read(mstosamps_97_63, 0);
        number mul_99_65 = tap_98_64 * in5;
        number add_100_66 = mul_99_65 + mul_96_62;
        number gen_101_67 = add_100_66;
        number history_22_next_102_68 = fixdenorm(add_100_66);
        number float_103_69 = float(3.6);
        number float_104_70 = float(4.8);
        number tap_105_71 = this->gen_03_delay_23_read(in4, 0);
        number mul_107_73 = mul_106_72 * __gen_03_history_25_value;
        auto mstosamps_108_74 = this->mstosamps(float_104_70);
        number tap_109_75 = this->gen_03_delay_24_read(mstosamps_108_74, 0);
        number mul_110_76 = tap_109_75 * in5;
        number add_111_77 = mul_110_76 + mul_107_73;
        number gen_112_78 = add_111_77;
        number history_25_next_113_79 = fixdenorm(add_111_77);
        number mul_115_81 = mul_114_80 * __gen_03_history_27_value;
        auto mstosamps_116_82 = this->mstosamps(float_103_69);
        number tap_117_83 = this->gen_03_delay_26_read(mstosamps_116_82, 0);
        number mul_118_84 = tap_117_83 * in5;
        number add_119_85 = mul_118_84 + mul_115_81;
        number gen_120_86 = add_119_85;
        number history_27_next_121_87 = fixdenorm(add_119_85);
        number mul_123_89 = mul_122_88 * __gen_03_history_29_value;
        number tap_125_91 = this->gen_03_delay_28_read(mstosamps_124_90, 0);
        number mul_126_92 = tap_125_91 * in5;
        number add_127_93 = mul_126_92 + mul_123_89;
        number gen_128_94 = add_127_93;
        number history_29_next_129_95 = fixdenorm(add_127_93);
        number mul_131_97 = mul_130_96 * __gen_03_history_31_value;
        auto mstosamps_132_98 = this->mstosamps(float_67_33);
        number tap_133_99 = this->gen_03_delay_30_read(mstosamps_132_98, 0);
        number mul_134_100 = tap_133_99 * in5;
        number add_135_101 = mul_134_100 + mul_131_97;
        number gen_136_102 = add_135_101;
        number history_31_next_137_103 = fixdenorm(add_135_101);
        number add_138_104 = gen_136_102 + tanh_70_36;
        number add_139_105 = gen_136_102 + tanh_72_38;
        number history_32_next_140_106 = fixdenorm(mix_79_45);
        number history_33_next_141_107 = fixdenorm(mix_94_60);
        this->gen_03_delay_1_write(tap_35_1);
        __gen_03_history_33_value = history_33_next_141_107;
        __gen_03_history_32_value = history_32_next_140_106;
        __gen_03_history_31_value = history_31_next_137_103;
        this->gen_03_delay_30_write(gen_128_94);
        __gen_03_history_29_value = history_29_next_129_95;
        this->gen_03_delay_28_write(gen_120_86);
        __gen_03_history_27_value = history_27_next_121_87;
        this->gen_03_delay_26_write(gen_112_78);
        __gen_03_history_25_value = history_25_next_113_79;
        this->gen_03_delay_24_write(tap_105_71);
        this->gen_03_delay_23_write(in1[(Index)i]);
        __gen_03_history_22_value = history_22_next_102_68;
        this->gen_03_delay_21_write(mix_94_60);
        this->gen_03_delay_20_write(add_139_105);
        __gen_03_history_19_value = history_19_next_87_53;
        this->gen_03_delay_18_write(mix_79_45);
        this->gen_03_delay_17_write(add_138_104);
        this->gen_03_delay_16_write(tap_93_59);
        this->gen_03_delay_15_write(tap_93_59);
        this->gen_03_delay_14_write(tap_78_44);
        this->gen_03_delay_13_write(gen_86_52);
        this->gen_03_delay_12_write(gen_101_67);
        this->gen_03_delay_11_write(tap_36_2);
        this->gen_03_delay_10_write(tap_36_2);
        this->gen_03_delay_9_write(gen_101_67);
        this->gen_03_delay_8_write(tap_93_59);
        this->gen_03_delay_7_write(tap_35_1);
        this->gen_03_delay_6_write(gen_86_52);
        this->gen_03_delay_5_write(tap_78_44);
        this->gen_03_delay_4_write(tap_78_44);
        this->gen_03_delay_3_write(gen_101_67);
        this->gen_03_delay_2_write(gen_86_52);
        this->gen_03_delay_1_step();
        this->gen_03_delay_2_step();
        this->gen_03_delay_3_step();
        this->gen_03_delay_4_step();
        this->gen_03_delay_5_step();
        this->gen_03_delay_6_step();
        this->gen_03_delay_7_step();
        this->gen_03_delay_8_step();
        this->gen_03_delay_9_step();
        this->gen_03_delay_10_step();
        this->gen_03_delay_11_step();
        this->gen_03_delay_12_step();
        this->gen_03_delay_13_step();
        this->gen_03_delay_14_step();
        this->gen_03_delay_15_step();
        this->gen_03_delay_16_step();
        this->gen_03_delay_17_step();
        this->gen_03_delay_18_step();
        this->gen_03_delay_20_step();
        this->gen_03_delay_21_step();
        this->gen_03_delay_23_step();
        this->gen_03_delay_24_step();
        this->gen_03_delay_26_step();
        this->gen_03_delay_28_step();
        this->gen_03_delay_30_step();
    }

    this->gen_03_history_32_value = __gen_03_history_32_value;
    this->gen_03_history_19_value = __gen_03_history_19_value;
    this->gen_03_history_33_value = __gen_03_history_33_value;
    this->gen_03_history_22_value = __gen_03_history_22_value;
    this->gen_03_history_25_value = __gen_03_history_25_value;
    this->gen_03_history_27_value = __gen_03_history_27_value;
    this->gen_03_history_29_value = __gen_03_history_29_value;
    this->gen_03_history_31_value = __gen_03_history_31_value;
}

void signaladder_13_perform(
    const SampleValue * in1,
    const SampleValue * in2,
    SampleValue * out,
    Index n
) {
    Index i;

    for (i = 0; i < n; i++) {
        out[(Index)i] = in1[(Index)i] + in2[(Index)i];
    }
}

void dspexpr_05_perform(const Sample * in1, number in2, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] * in2;//#map:_###_obj_###_:1
    }
}

void signaladder_14_perform(
    const SampleValue * in1,
    const SampleValue * in2,
    const SampleValue * in3,
    const SampleValue * in4,
    const SampleValue * in5,
    SampleValue * out,
    Index n
) {
    Index i;

    for (i = 0; i < n; i++) {
        out[(Index)i] = in1[(Index)i] + in2[(Index)i] + in3[(Index)i] + in4[(Index)i] + in5[(Index)i];
    }
}

void dspexpr_04_perform(const Sample * in1, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = rnbo_tanh(in1[(Index)i]);//#map:_###_obj_###_:1
    }
}

void gen_02_perform(const Sample * in1, number in2, number in3, SampleValue * out1, Index n) {
    auto __gen_02_history_2_value = this->gen_02_history_2_value;
    auto __gen_02_history_1_value = this->gen_02_history_1_value;
    number div_3_0 = in3 / (number)127;
    number div_4_1 = in2 / (number)127;
    Index i;

    for (i = 0; i < n; i++) {
        number mix_5_2 = in1[(Index)i] + div_4_1 * (__gen_02_history_1_value - in1[(Index)i]);
        number mix_6_3 = mix_5_2 + div_3_0 * (__gen_02_history_2_value - mix_5_2);
        number sub_7_4 = mix_6_3 - 0;
        out1[(Index)i] = sub_7_4;
        number history_1_next_8_5 = fixdenorm(mix_5_2);
        number history_2_next_9_6 = fixdenorm(mix_6_3);
        __gen_02_history_1_value = history_1_next_8_5;
        __gen_02_history_2_value = history_2_next_9_6;
    }

    this->gen_02_history_1_value = __gen_02_history_1_value;
    this->gen_02_history_2_value = __gen_02_history_2_value;
}

void signalforwarder_01_perform(const SampleValue * input, SampleValue * output, Index n) {
    for (Index i = 0; i < n; i++) {
        output[(Index)i] = input[(Index)i];
    }
}

void signaladder_15_perform(
    const SampleValue * in1,
    const SampleValue * in2,
    SampleValue * out,
    Index n
) {
    Index i;

    for (i = 0; i < n; i++) {
        out[(Index)i] = in1[(Index)i] + in2[(Index)i];
    }
}

void metertilde_01_perform(const SampleValue * input, Index n) {
    auto __metertilde_01_currentIntervalInSamples = this->metertilde_01_currentIntervalInSamples;
    auto __metertilde_01_currentInterval = this->metertilde_01_currentInterval;
    auto __metertilde_01_currentPeak = this->metertilde_01_currentPeak;

    for (Index i = 0; i < n; i++) {
        number value = (input[(Index)i] < 0 ? -input[(Index)i] : input[(Index)i]);

        if (value > __metertilde_01_currentPeak)
            __metertilde_01_currentPeak = value;
    }

    __metertilde_01_currentInterval -= n;

    if (__metertilde_01_currentInterval <= 0) {
        __metertilde_01_currentInterval = __metertilde_01_currentIntervalInSamples;

        this->getEngine()->sendNumMessage(
            TAG("peak"),
            TAG("meter~_obj-100"),
            __metertilde_01_currentPeak,
            this->_currentTime
        );;

        this->getEngine()->scheduleClockEventWithValue(
            this,
            1777613927,
            this->sampsToMs((SampleIndex)(this->vs)) + this->_currentTime,
            __metertilde_01_currentPeak
        );;

        __metertilde_01_currentPeak = 0;
    }

    this->metertilde_01_currentPeak = __metertilde_01_currentPeak;
    this->metertilde_01_currentInterval = __metertilde_01_currentInterval;
}

void stackprotect_perform(Index n) {
    RNBO_UNUSED(n);
    auto __stackprotect_count = this->stackprotect_count;
    __stackprotect_count = 0;
    this->stackprotect_count = __stackprotect_count;
}

void numberobj_01_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 69) {
        localvalue = 69;
    }

    if ((bool)(true) && localvalue > 177) {
        localvalue = 177;
    }

    if (this->numberobj_01_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_01_value = localvalue;
}

void numberobj_02_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 0.999) {
        localvalue = 0.999;
    }

    if (this->numberobj_02_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_02_value = localvalue;
}

void numberobj_03_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_03_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_03_value = localvalue;
}

void numberobj_04_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_04_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_04_value = localvalue;
}

void numberobj_05_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_05_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_05_value = localvalue;
}

void numberobj_06_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_06_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_06_value = localvalue;
}

void numberobj_07_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 2) {
        localvalue = 2;
    }

    if (this->numberobj_07_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_07_value = localvalue;
}

void numberobj_08_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 12) {
        localvalue = 12;
    }

    if (this->numberobj_08_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_08_value = localvalue;
}

void numberobj_09_value_setter(number v) {
    number localvalue = v;

    if (this->numberobj_09_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_09_value = localvalue;
}

void numberobj_10_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_10_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_10_value = localvalue;
}

void numberobj_11_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 50) {
        localvalue = 50;
    }

    if (this->numberobj_11_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_11_value = localvalue;
}

void numberobj_12_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 131) {
        localvalue = 131;
    }

    if ((bool)(true) && localvalue > 336) {
        localvalue = 336;
    }

    if (this->numberobj_12_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_12_value = localvalue;
}

void numberobj_13_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 0.999) {
        localvalue = 0.999;
    }

    if (this->numberobj_13_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_13_value = localvalue;
}

void numberobj_14_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 1) {
        localvalue = 1;
    }

    if (this->numberobj_14_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_14_value = localvalue;
}

void numberobj_15_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_15_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_15_value = localvalue;
}

void numberobj_16_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_16_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_16_value = localvalue;
}

void numberobj_17_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_17_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_17_value = localvalue;
}

void numberobj_18_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 2000) {
        localvalue = 2000;
    }

    if (this->numberobj_18_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_18_value = localvalue;
}

void numberobj_19_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_19_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_19_value = localvalue;
}

void numberobj_20_value_setter(number v) {
    number localvalue = v;

    if (this->numberobj_20_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_20_value = localvalue;
}

void numberobj_21_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0.5) {
        localvalue = 0.5;
    }

    if ((bool)(true) && localvalue > 0.8) {
        localvalue = 0.8;
    }

    if (this->numberobj_21_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_21_value = localvalue;
}

void numberobj_22_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 2) {
        localvalue = 2;
    }

    if (this->numberobj_22_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_22_value = localvalue;
}

void numberobj_23_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 1) {
        localvalue = 1;
    }

    if (this->numberobj_23_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_23_value = localvalue;
}

void numberobj_24_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 0.85) {
        localvalue = 0.85;
    }

    if (this->numberobj_24_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_24_value = localvalue;
}

void numberobj_25_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 1) {
        localvalue = 1;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_25_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_25_value = localvalue;
}

void numberobj_26_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 1) {
        localvalue = 1;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_26_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_26_value = localvalue;
}

void numberobj_27_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 189) {
        localvalue = 189;
    }

    if ((bool)(true) && localvalue > 487) {
        localvalue = 487;
    }

    if (this->numberobj_27_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_27_value = localvalue;
}

void numberobj_28_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 0.999) {
        localvalue = 0.999;
    }

    if (this->numberobj_28_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_28_value = localvalue;
}

void numberobj_29_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_29_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_29_value = localvalue;
}

void numberobj_30_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_30_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_30_value = localvalue;
}

void numberobj_31_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_31_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_31_value = localvalue;
}

void numberobj_32_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_32_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_32_value = localvalue;
}

void numberobj_33_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 2) {
        localvalue = 2;
    }

    if (this->numberobj_33_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_33_value = localvalue;
}

void numberobj_34_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 1) {
        localvalue = 1;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_34_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_34_value = localvalue;
}

void numberobj_35_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 1) {
        localvalue = 1;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_35_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_35_value = localvalue;
}

void numberobj_36_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_36_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_36_value = localvalue;
}

void numberobj_37_value_setter(number v) {
    number localvalue = v;

    if ((bool)(true) && localvalue < 0) {
        localvalue = 0;
    }

    if ((bool)(true) && localvalue > 127) {
        localvalue = 127;
    }

    if (this->numberobj_37_currentFormat != 6) {
        localvalue = trunc(localvalue);
    }

    this->numberobj_37_value = localvalue;
}

number gen_01_history_1_getvalue() {
    return this->gen_01_history_1_value;
}

void gen_01_history_1_setvalue(number val) {
    this->gen_01_history_1_value = val;
}

void gen_01_history_1_reset() {
    this->gen_01_history_1_value = 0;
}

void gen_01_history_1_init() {
    this->gen_01_history_1_value = 0;
}

number gen_01_history_2_getvalue() {
    return this->gen_01_history_2_value;
}

void gen_01_history_2_setvalue(number val) {
    this->gen_01_history_2_value = val;
}

void gen_01_history_2_reset() {
    this->gen_01_history_2_value = 0;
}

void gen_01_history_2_init() {
    this->gen_01_history_2_value = 0;
}

void gen_01_delay_3_step() {
    this->gen_01_delay_3_reader++;

    if (this->gen_01_delay_3_reader >= (int)(this->gen_01_delay_3_buffer->getSize()))
        this->gen_01_delay_3_reader = 0;
}

number gen_01_delay_3_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_delay_3_buffer->getSize()) + this->gen_01_delay_3_reader - ((size > this->gen_01_delay_3__maxdelay ? this->gen_01_delay_3__maxdelay : (size < (this->gen_01_delay_3_reader != this->gen_01_delay_3_writer) ? this->gen_01_delay_3_reader != this->gen_01_delay_3_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_delay_3_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_delay_3_wrap))
        ), this->gen_01_delay_3_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_delay_3_wrap))
        ));
    }

    number r = (int)(this->gen_01_delay_3_buffer->getSize()) + this->gen_01_delay_3_reader - ((size > this->gen_01_delay_3__maxdelay ? this->gen_01_delay_3__maxdelay : (size < (this->gen_01_delay_3_reader != this->gen_01_delay_3_writer) ? this->gen_01_delay_3_reader != this->gen_01_delay_3_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_delay_3_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_delay_3_wrap))
    );
}

void gen_01_delay_3_write(number v) {
    this->gen_01_delay_3_writer = this->gen_01_delay_3_reader;
    this->gen_01_delay_3_buffer[(Index)this->gen_01_delay_3_writer] = v;
}

number gen_01_delay_3_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_delay_3__maxdelay : size);
    number val = this->gen_01_delay_3_read(effectiveSize, 0);
    this->gen_01_delay_3_write(v);
    this->gen_01_delay_3_step();
    return val;
}

array<Index, 2> gen_01_delay_3_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_delay_3_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_delay_3_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_delay_3_init() {
    auto result = this->gen_01_delay_3_calcSizeInSamples();
    this->gen_01_delay_3__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_3_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_delay_3_wrap = requestedSizeInSamples - 1;
}

void gen_01_delay_3_clear() {
    this->gen_01_delay_3_buffer->setZero();
}

void gen_01_delay_3_reset() {
    auto result = this->gen_01_delay_3_calcSizeInSamples();
    this->gen_01_delay_3__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_3_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_delay_3_buffer);
    this->gen_01_delay_3_wrap = this->gen_01_delay_3_buffer->getSize() - 1;
    this->gen_01_delay_3_clear();

    if (this->gen_01_delay_3_reader >= this->gen_01_delay_3__maxdelay || this->gen_01_delay_3_writer >= this->gen_01_delay_3__maxdelay) {
        this->gen_01_delay_3_reader = 0;
        this->gen_01_delay_3_writer = 0;
    }
}

void gen_01_delay_3_dspsetup() {
    this->gen_01_delay_3_reset();
}

number gen_01_delay_3_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_delay_3_size() {
    return this->gen_01_delay_3__maxdelay;
}

void gen_01_delay_4_step() {
    this->gen_01_delay_4_reader++;

    if (this->gen_01_delay_4_reader >= (int)(this->gen_01_delay_4_buffer->getSize()))
        this->gen_01_delay_4_reader = 0;
}

number gen_01_delay_4_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_delay_4_buffer->getSize()) + this->gen_01_delay_4_reader - ((size > this->gen_01_delay_4__maxdelay ? this->gen_01_delay_4__maxdelay : (size < (this->gen_01_delay_4_reader != this->gen_01_delay_4_writer) ? this->gen_01_delay_4_reader != this->gen_01_delay_4_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_delay_4_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_delay_4_wrap))
        ), this->gen_01_delay_4_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_delay_4_wrap))
        ));
    }

    number r = (int)(this->gen_01_delay_4_buffer->getSize()) + this->gen_01_delay_4_reader - ((size > this->gen_01_delay_4__maxdelay ? this->gen_01_delay_4__maxdelay : (size < (this->gen_01_delay_4_reader != this->gen_01_delay_4_writer) ? this->gen_01_delay_4_reader != this->gen_01_delay_4_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_delay_4_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_delay_4_wrap))
    );
}

void gen_01_delay_4_write(number v) {
    this->gen_01_delay_4_writer = this->gen_01_delay_4_reader;
    this->gen_01_delay_4_buffer[(Index)this->gen_01_delay_4_writer] = v;
}

number gen_01_delay_4_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_delay_4__maxdelay : size);
    number val = this->gen_01_delay_4_read(effectiveSize, 0);
    this->gen_01_delay_4_write(v);
    this->gen_01_delay_4_step();
    return val;
}

array<Index, 2> gen_01_delay_4_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_delay_4_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_delay_4_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_delay_4_init() {
    auto result = this->gen_01_delay_4_calcSizeInSamples();
    this->gen_01_delay_4__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_4_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_delay_4_wrap = requestedSizeInSamples - 1;
}

void gen_01_delay_4_clear() {
    this->gen_01_delay_4_buffer->setZero();
}

void gen_01_delay_4_reset() {
    auto result = this->gen_01_delay_4_calcSizeInSamples();
    this->gen_01_delay_4__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_4_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_delay_4_buffer);
    this->gen_01_delay_4_wrap = this->gen_01_delay_4_buffer->getSize() - 1;
    this->gen_01_delay_4_clear();

    if (this->gen_01_delay_4_reader >= this->gen_01_delay_4__maxdelay || this->gen_01_delay_4_writer >= this->gen_01_delay_4__maxdelay) {
        this->gen_01_delay_4_reader = 0;
        this->gen_01_delay_4_writer = 0;
    }
}

void gen_01_delay_4_dspsetup() {
    this->gen_01_delay_4_reset();
}

number gen_01_delay_4_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_delay_4_size() {
    return this->gen_01_delay_4__maxdelay;
}

number gen_01_history_5_getvalue() {
    return this->gen_01_history_5_value;
}

void gen_01_history_5_setvalue(number val) {
    this->gen_01_history_5_value = val;
}

void gen_01_history_5_reset() {
    this->gen_01_history_5_value = 0;
}

void gen_01_history_5_init() {
    this->gen_01_history_5_value = 0;
}

number gen_01_history_6_getvalue() {
    return this->gen_01_history_6_value;
}

void gen_01_history_6_setvalue(number val) {
    this->gen_01_history_6_value = val;
}

void gen_01_history_6_reset() {
    this->gen_01_history_6_value = 0;
}

void gen_01_history_6_init() {
    this->gen_01_history_6_value = 0;
}

number gen_01_history_7_getvalue() {
    return this->gen_01_history_7_value;
}

void gen_01_history_7_setvalue(number val) {
    this->gen_01_history_7_value = val;
}

void gen_01_history_7_reset() {
    this->gen_01_history_7_value = 0;
}

void gen_01_history_7_init() {
    this->gen_01_history_7_value = 0;
}

number gen_01_history_8_getvalue() {
    return this->gen_01_history_8_value;
}

void gen_01_history_8_setvalue(number val) {
    this->gen_01_history_8_value = val;
}

void gen_01_history_8_reset() {
    this->gen_01_history_8_value = 0;
}

void gen_01_history_8_init() {
    this->gen_01_history_8_value = 0;
}

number gen_01_history_9_getvalue() {
    return this->gen_01_history_9_value;
}

void gen_01_history_9_setvalue(number val) {
    this->gen_01_history_9_value = val;
}

void gen_01_history_9_reset() {
    this->gen_01_history_9_value = 0;
}

void gen_01_history_9_init() {
    this->gen_01_history_9_value = 0;
}

number gen_01_history_10_getvalue() {
    return this->gen_01_history_10_value;
}

void gen_01_history_10_setvalue(number val) {
    this->gen_01_history_10_value = val;
}

void gen_01_history_10_reset() {
    this->gen_01_history_10_value = 0;
}

void gen_01_history_10_init() {
    this->gen_01_history_10_value = 0;
}

number gen_01_history_11_getvalue() {
    return this->gen_01_history_11_value;
}

void gen_01_history_11_setvalue(number val) {
    this->gen_01_history_11_value = val;
}

void gen_01_history_11_reset() {
    this->gen_01_history_11_value = 0;
}

void gen_01_history_11_init() {
    this->gen_01_history_11_value = 0;
}

number gen_01_history_12_getvalue() {
    return this->gen_01_history_12_value;
}

void gen_01_history_12_setvalue(number val) {
    this->gen_01_history_12_value = val;
}

void gen_01_history_12_reset() {
    this->gen_01_history_12_value = 0;
}

void gen_01_history_12_init() {
    this->gen_01_history_12_value = 0;
}

void gen_01_dspsetup(bool force) {
    if ((bool)(this->gen_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->gen_01_setupDone = true;
    this->gen_01_delay_3_dspsetup();
    this->gen_01_delay_4_dspsetup();
}

void numberobj_01_init() {
    this->numberobj_01_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-14"), 1, this->_currentTime);
}

void numberobj_01_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_01_value;
}

void numberobj_01_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_01_value_set(preset["value"]);
}

void numberobj_02_init() {
    this->numberobj_02_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-17"), 1, this->_currentTime);
}

void numberobj_02_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_02_value;
}

void numberobj_02_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_02_value_set(preset["value"]);
}

void numberobj_03_init() {
    this->numberobj_03_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-59"), 1, this->_currentTime);
}

void numberobj_03_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_03_value;
}

void numberobj_03_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_03_value_set(preset["value"]);
}

void numberobj_04_init() {
    this->numberobj_04_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-61"), 1, this->_currentTime);
}

void numberobj_04_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_04_value;
}

void numberobj_04_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_04_value_set(preset["value"]);
}

void numberobj_05_init() {
    this->numberobj_05_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-65"), 1, this->_currentTime);
}

void numberobj_05_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_05_value;
}

void numberobj_05_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_05_value_set(preset["value"]);
}

void numberobj_06_init() {
    this->numberobj_06_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-64"), 1, this->_currentTime);
}

void numberobj_06_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_06_value;
}

void numberobj_06_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_06_value_set(preset["value"]);
}

void numberobj_07_init() {
    this->numberobj_07_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-162"), 1, this->_currentTime);
}

void numberobj_07_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_07_value;
}

void numberobj_07_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_07_value_set(preset["value"]);
}

void param_01_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_01_value;
}

void param_01_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_01_value_set(preset["value"]);
}

void param_02_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_02_value;
}

void param_02_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_02_value_set(preset["value"]);
}

number gen_02_history_1_getvalue() {
    return this->gen_02_history_1_value;
}

void gen_02_history_1_setvalue(number val) {
    this->gen_02_history_1_value = val;
}

void gen_02_history_1_reset() {
    this->gen_02_history_1_value = 0;
}

void gen_02_history_1_init() {
    this->gen_02_history_1_value = 0;
}

number gen_02_history_2_getvalue() {
    return this->gen_02_history_2_value;
}

void gen_02_history_2_setvalue(number val) {
    this->gen_02_history_2_value = val;
}

void gen_02_history_2_reset() {
    this->gen_02_history_2_value = 0;
}

void gen_02_history_2_init() {
    this->gen_02_history_2_value = 0;
}

void numberobj_08_init() {
    this->numberobj_08_currentFormat = 0;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-30"), 1, this->_currentTime);
}

void numberobj_08_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_08_value;
}

void numberobj_08_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_08_value_set(preset["value"]);
}

void numberobj_09_init() {
    this->numberobj_09_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-148"), 1, this->_currentTime);
}

void numberobj_09_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_09_value;
}

void numberobj_09_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_09_value_set(preset["value"]);
}

void gen_03_delay_1_step() {
    this->gen_03_delay_1_reader++;

    if (this->gen_03_delay_1_reader >= (int)(this->gen_03_delay_1_buffer->getSize()))
        this->gen_03_delay_1_reader = 0;
}

number gen_03_delay_1_read(number size, Int interp) {
    RNBO_UNUSED(interp);
    RNBO_UNUSED(size);

    {
        number r = (int)(this->gen_03_delay_1_buffer->getSize()) + this->gen_03_delay_1_reader - ((8 > this->gen_03_delay_1__maxdelay ? this->gen_03_delay_1__maxdelay : (8 < (this->gen_03_delay_1_reader != this->gen_03_delay_1_writer) ? this->gen_03_delay_1_reader != this->gen_03_delay_1_writer : 8)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_1_wrap))
        ), this->gen_03_delay_1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_1_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_1_buffer->getSize()) + this->gen_03_delay_1_reader - ((8 > this->gen_03_delay_1__maxdelay ? this->gen_03_delay_1__maxdelay : (8 < (this->gen_03_delay_1_reader != this->gen_03_delay_1_writer) ? this->gen_03_delay_1_reader != this->gen_03_delay_1_writer : 8)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_1_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_1_wrap))
    );
}

void gen_03_delay_1_write(number v) {
    this->gen_03_delay_1_writer = this->gen_03_delay_1_reader;
    this->gen_03_delay_1_buffer[(Index)this->gen_03_delay_1_writer] = v;
}

number gen_03_delay_1_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_1__maxdelay : size);
    number val = this->gen_03_delay_1_read(effectiveSize, 0);
    this->gen_03_delay_1_write(v);
    this->gen_03_delay_1_step();
    return val;
}

array<Index, 2> gen_03_delay_1_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_1_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_1_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_1_init() {
    auto result = this->gen_03_delay_1_calcSizeInSamples();
    this->gen_03_delay_1__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_1_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_1_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_1_clear() {
    this->gen_03_delay_1_buffer->setZero();
}

void gen_03_delay_1_reset() {
    auto result = this->gen_03_delay_1_calcSizeInSamples();
    this->gen_03_delay_1__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_1_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_1_buffer);
    this->gen_03_delay_1_wrap = this->gen_03_delay_1_buffer->getSize() - 1;
    this->gen_03_delay_1_clear();

    if (this->gen_03_delay_1_reader >= this->gen_03_delay_1__maxdelay || this->gen_03_delay_1_writer >= this->gen_03_delay_1__maxdelay) {
        this->gen_03_delay_1_reader = 0;
        this->gen_03_delay_1_writer = 0;
    }
}

void gen_03_delay_1_dspsetup() {
    this->gen_03_delay_1_reset();
}

number gen_03_delay_1_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 4;
}

number gen_03_delay_1_size() {
    return this->gen_03_delay_1__maxdelay;
}

void gen_03_delay_2_step() {
    this->gen_03_delay_2_reader++;

    if (this->gen_03_delay_2_reader >= (int)(this->gen_03_delay_2_buffer->getSize()))
        this->gen_03_delay_2_reader = 0;
}

number gen_03_delay_2_read(number size, Int interp) {
    RNBO_UNUSED(interp);
    RNBO_UNUSED(size);

    {
        number r = (int)(this->gen_03_delay_2_buffer->getSize()) + this->gen_03_delay_2_reader - ((106 > this->gen_03_delay_2__maxdelay ? this->gen_03_delay_2__maxdelay : (106 < (this->gen_03_delay_2_reader != this->gen_03_delay_2_writer) ? this->gen_03_delay_2_reader != this->gen_03_delay_2_writer : 106)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_2_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_2_wrap))
        ), this->gen_03_delay_2_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_2_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_2_buffer->getSize()) + this->gen_03_delay_2_reader - ((106 > this->gen_03_delay_2__maxdelay ? this->gen_03_delay_2__maxdelay : (106 < (this->gen_03_delay_2_reader != this->gen_03_delay_2_writer) ? this->gen_03_delay_2_reader != this->gen_03_delay_2_writer : 106)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_2_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_2_wrap))
    );
}

void gen_03_delay_2_write(number v) {
    this->gen_03_delay_2_writer = this->gen_03_delay_2_reader;
    this->gen_03_delay_2_buffer[(Index)this->gen_03_delay_2_writer] = v;
}

number gen_03_delay_2_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_2__maxdelay : size);
    number val = this->gen_03_delay_2_read(effectiveSize, 0);
    this->gen_03_delay_2_write(v);
    this->gen_03_delay_2_step();
    return val;
}

array<Index, 2> gen_03_delay_2_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_2_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_2_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_2_init() {
    auto result = this->gen_03_delay_2_calcSizeInSamples();
    this->gen_03_delay_2__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_2_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_2_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_2_clear() {
    this->gen_03_delay_2_buffer->setZero();
}

void gen_03_delay_2_reset() {
    auto result = this->gen_03_delay_2_calcSizeInSamples();
    this->gen_03_delay_2__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_2_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_2_buffer);
    this->gen_03_delay_2_wrap = this->gen_03_delay_2_buffer->getSize() - 1;
    this->gen_03_delay_2_clear();

    if (this->gen_03_delay_2_reader >= this->gen_03_delay_2__maxdelay || this->gen_03_delay_2_writer >= this->gen_03_delay_2__maxdelay) {
        this->gen_03_delay_2_reader = 0;
        this->gen_03_delay_2_writer = 0;
    }
}

void gen_03_delay_2_dspsetup() {
    this->gen_03_delay_2_reset();
}

number gen_03_delay_2_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 106;
}

number gen_03_delay_2_size() {
    return this->gen_03_delay_2__maxdelay;
}

void gen_03_delay_3_step() {
    this->gen_03_delay_3_reader++;

    if (this->gen_03_delay_3_reader >= (int)(this->gen_03_delay_3_buffer->getSize()))
        this->gen_03_delay_3_reader = 0;
}

number gen_03_delay_3_read(number size, Int interp) {
    RNBO_UNUSED(interp);
    RNBO_UNUSED(size);

    {
        number r = (int)(this->gen_03_delay_3_buffer->getSize()) + this->gen_03_delay_3_reader - ((125 > this->gen_03_delay_3__maxdelay ? this->gen_03_delay_3__maxdelay : (125 < (this->gen_03_delay_3_reader != this->gen_03_delay_3_writer) ? this->gen_03_delay_3_reader != this->gen_03_delay_3_writer : 125)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_3_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_3_wrap))
        ), this->gen_03_delay_3_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_3_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_3_buffer->getSize()) + this->gen_03_delay_3_reader - ((125 > this->gen_03_delay_3__maxdelay ? this->gen_03_delay_3__maxdelay : (125 < (this->gen_03_delay_3_reader != this->gen_03_delay_3_writer) ? this->gen_03_delay_3_reader != this->gen_03_delay_3_writer : 125)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_3_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_3_wrap))
    );
}

void gen_03_delay_3_write(number v) {
    this->gen_03_delay_3_writer = this->gen_03_delay_3_reader;
    this->gen_03_delay_3_buffer[(Index)this->gen_03_delay_3_writer] = v;
}

number gen_03_delay_3_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_3__maxdelay : size);
    number val = this->gen_03_delay_3_read(effectiveSize, 0);
    this->gen_03_delay_3_write(v);
    this->gen_03_delay_3_step();
    return val;
}

array<Index, 2> gen_03_delay_3_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_3_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_3_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_3_init() {
    auto result = this->gen_03_delay_3_calcSizeInSamples();
    this->gen_03_delay_3__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_3_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_3_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_3_clear() {
    this->gen_03_delay_3_buffer->setZero();
}

void gen_03_delay_3_reset() {
    auto result = this->gen_03_delay_3_calcSizeInSamples();
    this->gen_03_delay_3__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_3_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_3_buffer);
    this->gen_03_delay_3_wrap = this->gen_03_delay_3_buffer->getSize() - 1;
    this->gen_03_delay_3_clear();

    if (this->gen_03_delay_3_reader >= this->gen_03_delay_3__maxdelay || this->gen_03_delay_3_writer >= this->gen_03_delay_3__maxdelay) {
        this->gen_03_delay_3_reader = 0;
        this->gen_03_delay_3_writer = 0;
    }
}

void gen_03_delay_3_dspsetup() {
    this->gen_03_delay_3_reset();
}

number gen_03_delay_3_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 125;
}

number gen_03_delay_3_size() {
    return this->gen_03_delay_3__maxdelay;
}

void gen_03_delay_4_step() {
    this->gen_03_delay_4_reader++;

    if (this->gen_03_delay_4_reader >= (int)(this->gen_03_delay_4_buffer->getSize()))
        this->gen_03_delay_4_reader = 0;
}

number gen_03_delay_4_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_03_delay_4_buffer->getSize()) + this->gen_03_delay_4_reader - ((size > this->gen_03_delay_4__maxdelay ? this->gen_03_delay_4__maxdelay : (size < (this->gen_03_delay_4_reader != this->gen_03_delay_4_writer) ? this->gen_03_delay_4_reader != this->gen_03_delay_4_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_4_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_4_wrap))
        ), this->gen_03_delay_4_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_4_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_4_buffer->getSize()) + this->gen_03_delay_4_reader - ((size > this->gen_03_delay_4__maxdelay ? this->gen_03_delay_4__maxdelay : (size < (this->gen_03_delay_4_reader != this->gen_03_delay_4_writer) ? this->gen_03_delay_4_reader != this->gen_03_delay_4_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_4_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_4_wrap))
    );
}

void gen_03_delay_4_write(number v) {
    this->gen_03_delay_4_writer = this->gen_03_delay_4_reader;
    this->gen_03_delay_4_buffer[(Index)this->gen_03_delay_4_writer] = v;
}

number gen_03_delay_4_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_4__maxdelay : size);
    number val = this->gen_03_delay_4_read(effectiveSize, 0);
    this->gen_03_delay_4_write(v);
    this->gen_03_delay_4_step();
    return val;
}

array<Index, 2> gen_03_delay_4_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_4_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_4_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_4_init() {
    auto result = this->gen_03_delay_4_calcSizeInSamples();
    this->gen_03_delay_4__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_4_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_4_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_4_clear() {
    this->gen_03_delay_4_buffer->setZero();
}

void gen_03_delay_4_reset() {
    auto result = this->gen_03_delay_4_calcSizeInSamples();
    this->gen_03_delay_4__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_4_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_4_buffer);
    this->gen_03_delay_4_wrap = this->gen_03_delay_4_buffer->getSize() - 1;
    this->gen_03_delay_4_clear();

    if (this->gen_03_delay_4_reader >= this->gen_03_delay_4__maxdelay || this->gen_03_delay_4_writer >= this->gen_03_delay_4__maxdelay) {
        this->gen_03_delay_4_reader = 0;
        this->gen_03_delay_4_writer = 0;
    }
}

void gen_03_delay_4_dspsetup() {
    this->gen_03_delay_4_reset();
}

number gen_03_delay_4_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 9;
}

number gen_03_delay_4_size() {
    return this->gen_03_delay_4__maxdelay;
}

void gen_03_delay_5_step() {
    this->gen_03_delay_5_reader++;

    if (this->gen_03_delay_5_reader >= (int)(this->gen_03_delay_5_buffer->getSize()))
        this->gen_03_delay_5_reader = 0;
}

number gen_03_delay_5_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_03_delay_5_buffer->getSize()) + this->gen_03_delay_5_reader - ((size > this->gen_03_delay_5__maxdelay ? this->gen_03_delay_5__maxdelay : (size < (this->gen_03_delay_5_reader != this->gen_03_delay_5_writer) ? this->gen_03_delay_5_reader != this->gen_03_delay_5_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_5_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_5_wrap))
        ), this->gen_03_delay_5_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_5_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_5_buffer->getSize()) + this->gen_03_delay_5_reader - ((size > this->gen_03_delay_5__maxdelay ? this->gen_03_delay_5__maxdelay : (size < (this->gen_03_delay_5_reader != this->gen_03_delay_5_writer) ? this->gen_03_delay_5_reader != this->gen_03_delay_5_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_5_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_5_wrap))
    );
}

void gen_03_delay_5_write(number v) {
    this->gen_03_delay_5_writer = this->gen_03_delay_5_reader;
    this->gen_03_delay_5_buffer[(Index)this->gen_03_delay_5_writer] = v;
}

number gen_03_delay_5_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_5__maxdelay : size);
    number val = this->gen_03_delay_5_read(effectiveSize, 0);
    this->gen_03_delay_5_write(v);
    this->gen_03_delay_5_step();
    return val;
}

array<Index, 2> gen_03_delay_5_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_5_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_5_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_5_init() {
    auto result = this->gen_03_delay_5_calcSizeInSamples();
    this->gen_03_delay_5__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_5_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_5_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_5_clear() {
    this->gen_03_delay_5_buffer->setZero();
}

void gen_03_delay_5_reset() {
    auto result = this->gen_03_delay_5_calcSizeInSamples();
    this->gen_03_delay_5__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_5_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_5_buffer);
    this->gen_03_delay_5_wrap = this->gen_03_delay_5_buffer->getSize() - 1;
    this->gen_03_delay_5_clear();

    if (this->gen_03_delay_5_reader >= this->gen_03_delay_5__maxdelay || this->gen_03_delay_5_writer >= this->gen_03_delay_5__maxdelay) {
        this->gen_03_delay_5_reader = 0;
        this->gen_03_delay_5_writer = 0;
    }
}

void gen_03_delay_5_dspsetup() {
    this->gen_03_delay_5_reset();
}

number gen_03_delay_5_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 99;
}

number gen_03_delay_5_size() {
    return this->gen_03_delay_5__maxdelay;
}

void gen_03_delay_6_step() {
    this->gen_03_delay_6_reader++;

    if (this->gen_03_delay_6_reader >= (int)(this->gen_03_delay_6_buffer->getSize()))
        this->gen_03_delay_6_reader = 0;
}

number gen_03_delay_6_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_03_delay_6_buffer->getSize()) + this->gen_03_delay_6_reader - ((size > this->gen_03_delay_6__maxdelay ? this->gen_03_delay_6__maxdelay : (size < (this->gen_03_delay_6_reader != this->gen_03_delay_6_writer) ? this->gen_03_delay_6_reader != this->gen_03_delay_6_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_6_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_6_wrap))
        ), this->gen_03_delay_6_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_6_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_6_buffer->getSize()) + this->gen_03_delay_6_reader - ((size > this->gen_03_delay_6__maxdelay ? this->gen_03_delay_6__maxdelay : (size < (this->gen_03_delay_6_reader != this->gen_03_delay_6_writer) ? this->gen_03_delay_6_reader != this->gen_03_delay_6_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_6_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_6_wrap))
    );
}

void gen_03_delay_6_write(number v) {
    this->gen_03_delay_6_writer = this->gen_03_delay_6_reader;
    this->gen_03_delay_6_buffer[(Index)this->gen_03_delay_6_writer] = v;
}

number gen_03_delay_6_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_6__maxdelay : size);
    number val = this->gen_03_delay_6_read(effectiveSize, 0);
    this->gen_03_delay_6_write(v);
    this->gen_03_delay_6_step();
    return val;
}

array<Index, 2> gen_03_delay_6_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_6_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_6_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_6_init() {
    auto result = this->gen_03_delay_6_calcSizeInSamples();
    this->gen_03_delay_6__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_6_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_6_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_6_clear() {
    this->gen_03_delay_6_buffer->setZero();
}

void gen_03_delay_6_reset() {
    auto result = this->gen_03_delay_6_calcSizeInSamples();
    this->gen_03_delay_6__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_6_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_6_buffer);
    this->gen_03_delay_6_wrap = this->gen_03_delay_6_buffer->getSize() - 1;
    this->gen_03_delay_6_clear();

    if (this->gen_03_delay_6_reader >= this->gen_03_delay_6__maxdelay || this->gen_03_delay_6_writer >= this->gen_03_delay_6__maxdelay) {
        this->gen_03_delay_6_reader = 0;
        this->gen_03_delay_6_writer = 0;
    }
}

void gen_03_delay_6_dspsetup() {
    this->gen_03_delay_6_reset();
}

number gen_03_delay_6_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 64;
}

number gen_03_delay_6_size() {
    return this->gen_03_delay_6__maxdelay;
}

void gen_03_delay_7_step() {
    this->gen_03_delay_7_reader++;

    if (this->gen_03_delay_7_reader >= (int)(this->gen_03_delay_7_buffer->getSize()))
        this->gen_03_delay_7_reader = 0;
}

number gen_03_delay_7_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_03_delay_7_buffer->getSize()) + this->gen_03_delay_7_reader - ((size > this->gen_03_delay_7__maxdelay ? this->gen_03_delay_7__maxdelay : (size < (this->gen_03_delay_7_reader != this->gen_03_delay_7_writer) ? this->gen_03_delay_7_reader != this->gen_03_delay_7_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_7_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_7_wrap))
        ), this->gen_03_delay_7_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_7_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_7_buffer->getSize()) + this->gen_03_delay_7_reader - ((size > this->gen_03_delay_7__maxdelay ? this->gen_03_delay_7__maxdelay : (size < (this->gen_03_delay_7_reader != this->gen_03_delay_7_writer) ? this->gen_03_delay_7_reader != this->gen_03_delay_7_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_7_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_7_wrap))
    );
}

void gen_03_delay_7_write(number v) {
    this->gen_03_delay_7_writer = this->gen_03_delay_7_reader;
    this->gen_03_delay_7_buffer[(Index)this->gen_03_delay_7_writer] = v;
}

number gen_03_delay_7_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_7__maxdelay : size);
    number val = this->gen_03_delay_7_read(effectiveSize, 0);
    this->gen_03_delay_7_write(v);
    this->gen_03_delay_7_step();
    return val;
}

array<Index, 2> gen_03_delay_7_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_7_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_7_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_7_init() {
    auto result = this->gen_03_delay_7_calcSizeInSamples();
    this->gen_03_delay_7__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_7_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_7_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_7_clear() {
    this->gen_03_delay_7_buffer->setZero();
}

void gen_03_delay_7_reset() {
    auto result = this->gen_03_delay_7_calcSizeInSamples();
    this->gen_03_delay_7__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_7_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_7_buffer);
    this->gen_03_delay_7_wrap = this->gen_03_delay_7_buffer->getSize() - 1;
    this->gen_03_delay_7_clear();

    if (this->gen_03_delay_7_reader >= this->gen_03_delay_7__maxdelay || this->gen_03_delay_7_writer >= this->gen_03_delay_7__maxdelay) {
        this->gen_03_delay_7_reader = 0;
        this->gen_03_delay_7_writer = 0;
    }
}

void gen_03_delay_7_dspsetup() {
    this->gen_03_delay_7_reset();
}

number gen_03_delay_7_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 67;
}

number gen_03_delay_7_size() {
    return this->gen_03_delay_7__maxdelay;
}

void gen_03_delay_8_step() {
    this->gen_03_delay_8_reader++;

    if (this->gen_03_delay_8_reader >= (int)(this->gen_03_delay_8_buffer->getSize()))
        this->gen_03_delay_8_reader = 0;
}

number gen_03_delay_8_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_03_delay_8_buffer->getSize()) + this->gen_03_delay_8_reader - ((size > this->gen_03_delay_8__maxdelay ? this->gen_03_delay_8__maxdelay : (size < (this->gen_03_delay_8_reader != this->gen_03_delay_8_writer) ? this->gen_03_delay_8_reader != this->gen_03_delay_8_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_8_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_8_wrap))
        ), this->gen_03_delay_8_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_8_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_8_buffer->getSize()) + this->gen_03_delay_8_reader - ((size > this->gen_03_delay_8__maxdelay ? this->gen_03_delay_8__maxdelay : (size < (this->gen_03_delay_8_reader != this->gen_03_delay_8_writer) ? this->gen_03_delay_8_reader != this->gen_03_delay_8_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_8_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_8_wrap))
    );
}

void gen_03_delay_8_write(number v) {
    this->gen_03_delay_8_writer = this->gen_03_delay_8_reader;
    this->gen_03_delay_8_buffer[(Index)this->gen_03_delay_8_writer] = v;
}

number gen_03_delay_8_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_8__maxdelay : size);
    number val = this->gen_03_delay_8_read(effectiveSize, 0);
    this->gen_03_delay_8_write(v);
    this->gen_03_delay_8_step();
    return val;
}

array<Index, 2> gen_03_delay_8_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_8_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_8_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_8_init() {
    auto result = this->gen_03_delay_8_calcSizeInSamples();
    this->gen_03_delay_8__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_8_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_8_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_8_clear() {
    this->gen_03_delay_8_buffer->setZero();
}

void gen_03_delay_8_reset() {
    auto result = this->gen_03_delay_8_calcSizeInSamples();
    this->gen_03_delay_8__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_8_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_8_buffer);
    this->gen_03_delay_8_wrap = this->gen_03_delay_8_buffer->getSize() - 1;
    this->gen_03_delay_8_clear();

    if (this->gen_03_delay_8_reader >= this->gen_03_delay_8__maxdelay || this->gen_03_delay_8_writer >= this->gen_03_delay_8__maxdelay) {
        this->gen_03_delay_8_reader = 0;
        this->gen_03_delay_8_writer = 0;
    }
}

void gen_03_delay_8_dspsetup() {
    this->gen_03_delay_8_reset();
}

number gen_03_delay_8_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 67;
}

number gen_03_delay_8_size() {
    return this->gen_03_delay_8__maxdelay;
}

void gen_03_delay_9_step() {
    this->gen_03_delay_9_reader++;

    if (this->gen_03_delay_9_reader >= (int)(this->gen_03_delay_9_buffer->getSize()))
        this->gen_03_delay_9_reader = 0;
}

number gen_03_delay_9_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_03_delay_9_buffer->getSize()) + this->gen_03_delay_9_reader - ((size > this->gen_03_delay_9__maxdelay ? this->gen_03_delay_9__maxdelay : (size < (this->gen_03_delay_9_reader != this->gen_03_delay_9_writer) ? this->gen_03_delay_9_reader != this->gen_03_delay_9_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_9_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_9_wrap))
        ), this->gen_03_delay_9_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_9_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_9_buffer->getSize()) + this->gen_03_delay_9_reader - ((size > this->gen_03_delay_9__maxdelay ? this->gen_03_delay_9__maxdelay : (size < (this->gen_03_delay_9_reader != this->gen_03_delay_9_writer) ? this->gen_03_delay_9_reader != this->gen_03_delay_9_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_9_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_9_wrap))
    );
}

void gen_03_delay_9_write(number v) {
    this->gen_03_delay_9_writer = this->gen_03_delay_9_reader;
    this->gen_03_delay_9_buffer[(Index)this->gen_03_delay_9_writer] = v;
}

number gen_03_delay_9_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_9__maxdelay : size);
    number val = this->gen_03_delay_9_read(effectiveSize, 0);
    this->gen_03_delay_9_write(v);
    this->gen_03_delay_9_step();
    return val;
}

array<Index, 2> gen_03_delay_9_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_9_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_9_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_9_init() {
    auto result = this->gen_03_delay_9_calcSizeInSamples();
    this->gen_03_delay_9__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_9_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_9_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_9_clear() {
    this->gen_03_delay_9_buffer->setZero();
}

void gen_03_delay_9_reset() {
    auto result = this->gen_03_delay_9_calcSizeInSamples();
    this->gen_03_delay_9__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_9_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_9_buffer);
    this->gen_03_delay_9_wrap = this->gen_03_delay_9_buffer->getSize() - 1;
    this->gen_03_delay_9_clear();

    if (this->gen_03_delay_9_reader >= this->gen_03_delay_9__maxdelay || this->gen_03_delay_9_writer >= this->gen_03_delay_9__maxdelay) {
        this->gen_03_delay_9_reader = 0;
        this->gen_03_delay_9_writer = 0;
    }
}

void gen_03_delay_9_dspsetup() {
    this->gen_03_delay_9_reset();
}

number gen_03_delay_9_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 60;
}

number gen_03_delay_9_size() {
    return this->gen_03_delay_9__maxdelay;
}

void gen_03_delay_10_step() {
    this->gen_03_delay_10_reader++;

    if (this->gen_03_delay_10_reader >= (int)(this->gen_03_delay_10_buffer->getSize()))
        this->gen_03_delay_10_reader = 0;
}

number gen_03_delay_10_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_03_delay_10_buffer->getSize()) + this->gen_03_delay_10_reader - ((size > this->gen_03_delay_10__maxdelay ? this->gen_03_delay_10__maxdelay : (size < (this->gen_03_delay_10_reader != this->gen_03_delay_10_writer) ? this->gen_03_delay_10_reader != this->gen_03_delay_10_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_10_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_10_wrap))
        ), this->gen_03_delay_10_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_10_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_10_buffer->getSize()) + this->gen_03_delay_10_reader - ((size > this->gen_03_delay_10__maxdelay ? this->gen_03_delay_10__maxdelay : (size < (this->gen_03_delay_10_reader != this->gen_03_delay_10_writer) ? this->gen_03_delay_10_reader != this->gen_03_delay_10_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_10_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_10_wrap))
    );
}

void gen_03_delay_10_write(number v) {
    this->gen_03_delay_10_writer = this->gen_03_delay_10_reader;
    this->gen_03_delay_10_buffer[(Index)this->gen_03_delay_10_writer] = v;
}

number gen_03_delay_10_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_10__maxdelay : size);
    number val = this->gen_03_delay_10_read(effectiveSize, 0);
    this->gen_03_delay_10_write(v);
    this->gen_03_delay_10_step();
    return val;
}

array<Index, 2> gen_03_delay_10_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_10_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_10_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_10_init() {
    auto result = this->gen_03_delay_10_calcSizeInSamples();
    this->gen_03_delay_10__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_10_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_10_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_10_clear() {
    this->gen_03_delay_10_buffer->setZero();
}

void gen_03_delay_10_reset() {
    auto result = this->gen_03_delay_10_calcSizeInSamples();
    this->gen_03_delay_10__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_10_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_10_buffer);
    this->gen_03_delay_10_wrap = this->gen_03_delay_10_buffer->getSize() - 1;
    this->gen_03_delay_10_clear();

    if (this->gen_03_delay_10_reader >= this->gen_03_delay_10__maxdelay || this->gen_03_delay_10_writer >= this->gen_03_delay_10__maxdelay) {
        this->gen_03_delay_10_reader = 0;
        this->gen_03_delay_10_writer = 0;
    }
}

void gen_03_delay_10_dspsetup() {
    this->gen_03_delay_10_reset();
}

number gen_03_delay_10_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 67;
}

number gen_03_delay_10_size() {
    return this->gen_03_delay_10__maxdelay;
}

void gen_03_delay_11_step() {
    this->gen_03_delay_11_reader++;

    if (this->gen_03_delay_11_reader >= (int)(this->gen_03_delay_11_buffer->getSize()))
        this->gen_03_delay_11_reader = 0;
}

number gen_03_delay_11_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_03_delay_11_buffer->getSize()) + this->gen_03_delay_11_reader - ((size > this->gen_03_delay_11__maxdelay ? this->gen_03_delay_11__maxdelay : (size < (this->gen_03_delay_11_reader != this->gen_03_delay_11_writer) ? this->gen_03_delay_11_reader != this->gen_03_delay_11_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_11_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_11_wrap))
        ), this->gen_03_delay_11_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_11_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_11_buffer->getSize()) + this->gen_03_delay_11_reader - ((size > this->gen_03_delay_11__maxdelay ? this->gen_03_delay_11__maxdelay : (size < (this->gen_03_delay_11_reader != this->gen_03_delay_11_writer) ? this->gen_03_delay_11_reader != this->gen_03_delay_11_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_11_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_11_wrap))
    );
}

void gen_03_delay_11_write(number v) {
    this->gen_03_delay_11_writer = this->gen_03_delay_11_reader;
    this->gen_03_delay_11_buffer[(Index)this->gen_03_delay_11_writer] = v;
}

number gen_03_delay_11_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_11__maxdelay : size);
    number val = this->gen_03_delay_11_read(effectiveSize, 0);
    this->gen_03_delay_11_write(v);
    this->gen_03_delay_11_step();
    return val;
}

array<Index, 2> gen_03_delay_11_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_11_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_11_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_11_init() {
    auto result = this->gen_03_delay_11_calcSizeInSamples();
    this->gen_03_delay_11__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_11_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_11_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_11_clear() {
    this->gen_03_delay_11_buffer->setZero();
}

void gen_03_delay_11_reset() {
    auto result = this->gen_03_delay_11_calcSizeInSamples();
    this->gen_03_delay_11__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_11_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_11_buffer);
    this->gen_03_delay_11_wrap = this->gen_03_delay_11_buffer->getSize() - 1;
    this->gen_03_delay_11_clear();

    if (this->gen_03_delay_11_reader >= this->gen_03_delay_11__maxdelay || this->gen_03_delay_11_writer >= this->gen_03_delay_11__maxdelay) {
        this->gen_03_delay_11_reader = 0;
        this->gen_03_delay_11_writer = 0;
    }
}

void gen_03_delay_11_dspsetup() {
    this->gen_03_delay_11_reset();
}

number gen_03_delay_11_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 90;
}

number gen_03_delay_11_size() {
    return this->gen_03_delay_11__maxdelay;
}

void gen_03_delay_12_step() {
    this->gen_03_delay_12_reader++;

    if (this->gen_03_delay_12_reader >= (int)(this->gen_03_delay_12_buffer->getSize()))
        this->gen_03_delay_12_reader = 0;
}

number gen_03_delay_12_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_03_delay_12_buffer->getSize()) + this->gen_03_delay_12_reader - ((size > this->gen_03_delay_12__maxdelay ? this->gen_03_delay_12__maxdelay : (size < (this->gen_03_delay_12_reader != this->gen_03_delay_12_writer) ? this->gen_03_delay_12_reader != this->gen_03_delay_12_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_12_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_12_wrap))
        ), this->gen_03_delay_12_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_12_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_12_buffer->getSize()) + this->gen_03_delay_12_reader - ((size > this->gen_03_delay_12__maxdelay ? this->gen_03_delay_12__maxdelay : (size < (this->gen_03_delay_12_reader != this->gen_03_delay_12_writer) ? this->gen_03_delay_12_reader != this->gen_03_delay_12_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_12_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_12_wrap))
    );
}

void gen_03_delay_12_write(number v) {
    this->gen_03_delay_12_writer = this->gen_03_delay_12_reader;
    this->gen_03_delay_12_buffer[(Index)this->gen_03_delay_12_writer] = v;
}

number gen_03_delay_12_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_12__maxdelay : size);
    number val = this->gen_03_delay_12_read(effectiveSize, 0);
    this->gen_03_delay_12_write(v);
    this->gen_03_delay_12_step();
    return val;
}

array<Index, 2> gen_03_delay_12_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_12_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_12_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_12_init() {
    auto result = this->gen_03_delay_12_calcSizeInSamples();
    this->gen_03_delay_12__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_12_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_12_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_12_clear() {
    this->gen_03_delay_12_buffer->setZero();
}

void gen_03_delay_12_reset() {
    auto result = this->gen_03_delay_12_calcSizeInSamples();
    this->gen_03_delay_12__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_12_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_12_buffer);
    this->gen_03_delay_12_wrap = this->gen_03_delay_12_buffer->getSize() - 1;
    this->gen_03_delay_12_clear();

    if (this->gen_03_delay_12_reader >= this->gen_03_delay_12__maxdelay || this->gen_03_delay_12_writer >= this->gen_03_delay_12__maxdelay) {
        this->gen_03_delay_12_reader = 0;
        this->gen_03_delay_12_writer = 0;
    }
}

void gen_03_delay_12_dspsetup() {
    this->gen_03_delay_12_reset();
}

number gen_03_delay_12_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 41;
}

number gen_03_delay_12_size() {
    return this->gen_03_delay_12__maxdelay;
}

void gen_03_delay_13_step() {
    this->gen_03_delay_13_reader++;

    if (this->gen_03_delay_13_reader >= (int)(this->gen_03_delay_13_buffer->getSize()))
        this->gen_03_delay_13_reader = 0;
}

number gen_03_delay_13_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_03_delay_13_buffer->getSize()) + this->gen_03_delay_13_reader - ((size > this->gen_03_delay_13__maxdelay ? this->gen_03_delay_13__maxdelay : (size < (this->gen_03_delay_13_reader != this->gen_03_delay_13_writer) ? this->gen_03_delay_13_reader != this->gen_03_delay_13_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_13_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_13_wrap))
        ), this->gen_03_delay_13_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_13_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_13_buffer->getSize()) + this->gen_03_delay_13_reader - ((size > this->gen_03_delay_13__maxdelay ? this->gen_03_delay_13__maxdelay : (size < (this->gen_03_delay_13_reader != this->gen_03_delay_13_writer) ? this->gen_03_delay_13_reader != this->gen_03_delay_13_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_13_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_13_wrap))
    );
}

void gen_03_delay_13_write(number v) {
    this->gen_03_delay_13_writer = this->gen_03_delay_13_reader;
    this->gen_03_delay_13_buffer[(Index)this->gen_03_delay_13_writer] = v;
}

number gen_03_delay_13_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_13__maxdelay : size);
    number val = this->gen_03_delay_13_read(effectiveSize, 0);
    this->gen_03_delay_13_write(v);
    this->gen_03_delay_13_step();
    return val;
}

array<Index, 2> gen_03_delay_13_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_13_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_13_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_13_init() {
    auto result = this->gen_03_delay_13_calcSizeInSamples();
    this->gen_03_delay_13__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_13_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_13_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_13_clear() {
    this->gen_03_delay_13_buffer->setZero();
}

void gen_03_delay_13_reset() {
    auto result = this->gen_03_delay_13_calcSizeInSamples();
    this->gen_03_delay_13__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_13_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_13_buffer);
    this->gen_03_delay_13_wrap = this->gen_03_delay_13_buffer->getSize() - 1;
    this->gen_03_delay_13_clear();

    if (this->gen_03_delay_13_reader >= this->gen_03_delay_13__maxdelay || this->gen_03_delay_13_writer >= this->gen_03_delay_13__maxdelay) {
        this->gen_03_delay_13_reader = 0;
        this->gen_03_delay_13_writer = 0;
    }
}

void gen_03_delay_13_dspsetup() {
    this->gen_03_delay_13_reset();
}

number gen_03_delay_13_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 11;
}

number gen_03_delay_13_size() {
    return this->gen_03_delay_13__maxdelay;
}

void gen_03_delay_14_step() {
    this->gen_03_delay_14_reader++;

    if (this->gen_03_delay_14_reader >= (int)(this->gen_03_delay_14_buffer->getSize()))
        this->gen_03_delay_14_reader = 0;
}

number gen_03_delay_14_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_03_delay_14_buffer->getSize()) + this->gen_03_delay_14_reader - ((size > this->gen_03_delay_14__maxdelay ? this->gen_03_delay_14__maxdelay : (size < (this->gen_03_delay_14_reader != this->gen_03_delay_14_writer) ? this->gen_03_delay_14_reader != this->gen_03_delay_14_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_14_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_14_wrap))
        ), this->gen_03_delay_14_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_14_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_14_buffer->getSize()) + this->gen_03_delay_14_reader - ((size > this->gen_03_delay_14__maxdelay ? this->gen_03_delay_14__maxdelay : (size < (this->gen_03_delay_14_reader != this->gen_03_delay_14_writer) ? this->gen_03_delay_14_reader != this->gen_03_delay_14_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_14_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_14_wrap))
    );
}

void gen_03_delay_14_write(number v) {
    this->gen_03_delay_14_writer = this->gen_03_delay_14_reader;
    this->gen_03_delay_14_buffer[(Index)this->gen_03_delay_14_writer] = v;
}

number gen_03_delay_14_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_14__maxdelay : size);
    number val = this->gen_03_delay_14_read(effectiveSize, 0);
    this->gen_03_delay_14_write(v);
    this->gen_03_delay_14_step();
    return val;
}

array<Index, 2> gen_03_delay_14_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_14_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_14_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_14_init() {
    auto result = this->gen_03_delay_14_calcSizeInSamples();
    this->gen_03_delay_14__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_14_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_14_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_14_clear() {
    this->gen_03_delay_14_buffer->setZero();
}

void gen_03_delay_14_reset() {
    auto result = this->gen_03_delay_14_calcSizeInSamples();
    this->gen_03_delay_14__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_14_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_14_buffer);
    this->gen_03_delay_14_wrap = this->gen_03_delay_14_buffer->getSize() - 1;
    this->gen_03_delay_14_clear();

    if (this->gen_03_delay_14_reader >= this->gen_03_delay_14__maxdelay || this->gen_03_delay_14_writer >= this->gen_03_delay_14__maxdelay) {
        this->gen_03_delay_14_reader = 0;
        this->gen_03_delay_14_writer = 0;
    }
}

void gen_03_delay_14_dspsetup() {
    this->gen_03_delay_14_reset();
}

number gen_03_delay_14_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 71;
}

number gen_03_delay_14_size() {
    return this->gen_03_delay_14__maxdelay;
}

void gen_03_delay_15_step() {
    this->gen_03_delay_15_reader++;

    if (this->gen_03_delay_15_reader >= (int)(this->gen_03_delay_15_buffer->getSize()))
        this->gen_03_delay_15_reader = 0;
}

number gen_03_delay_15_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_03_delay_15_buffer->getSize()) + this->gen_03_delay_15_reader - ((size > this->gen_03_delay_15__maxdelay ? this->gen_03_delay_15__maxdelay : (size < (this->gen_03_delay_15_reader != this->gen_03_delay_15_writer) ? this->gen_03_delay_15_reader != this->gen_03_delay_15_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_15_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_15_wrap))
        ), this->gen_03_delay_15_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_15_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_15_buffer->getSize()) + this->gen_03_delay_15_reader - ((size > this->gen_03_delay_15__maxdelay ? this->gen_03_delay_15__maxdelay : (size < (this->gen_03_delay_15_reader != this->gen_03_delay_15_writer) ? this->gen_03_delay_15_reader != this->gen_03_delay_15_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_15_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_15_wrap))
    );
}

void gen_03_delay_15_write(number v) {
    this->gen_03_delay_15_writer = this->gen_03_delay_15_reader;
    this->gen_03_delay_15_buffer[(Index)this->gen_03_delay_15_writer] = v;
}

number gen_03_delay_15_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_15__maxdelay : size);
    number val = this->gen_03_delay_15_read(effectiveSize, 0);
    this->gen_03_delay_15_write(v);
    this->gen_03_delay_15_step();
    return val;
}

array<Index, 2> gen_03_delay_15_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_15_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_15_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_15_init() {
    auto result = this->gen_03_delay_15_calcSizeInSamples();
    this->gen_03_delay_15__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_15_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_15_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_15_clear() {
    this->gen_03_delay_15_buffer->setZero();
}

void gen_03_delay_15_reset() {
    auto result = this->gen_03_delay_15_calcSizeInSamples();
    this->gen_03_delay_15__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_15_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_15_buffer);
    this->gen_03_delay_15_wrap = this->gen_03_delay_15_buffer->getSize() - 1;
    this->gen_03_delay_15_clear();

    if (this->gen_03_delay_15_reader >= this->gen_03_delay_15__maxdelay || this->gen_03_delay_15_writer >= this->gen_03_delay_15__maxdelay) {
        this->gen_03_delay_15_reader = 0;
        this->gen_03_delay_15_writer = 0;
    }
}

void gen_03_delay_15_dspsetup() {
    this->gen_03_delay_15_reset();
}

number gen_03_delay_15_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 122;
}

number gen_03_delay_15_size() {
    return this->gen_03_delay_15__maxdelay;
}

void gen_03_delay_16_step() {
    this->gen_03_delay_16_reader++;

    if (this->gen_03_delay_16_reader >= (int)(this->gen_03_delay_16_buffer->getSize()))
        this->gen_03_delay_16_reader = 0;
}

number gen_03_delay_16_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_03_delay_16_buffer->getSize()) + this->gen_03_delay_16_reader - ((size > this->gen_03_delay_16__maxdelay ? this->gen_03_delay_16__maxdelay : (size < (this->gen_03_delay_16_reader != this->gen_03_delay_16_writer) ? this->gen_03_delay_16_reader != this->gen_03_delay_16_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_16_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_16_wrap))
        ), this->gen_03_delay_16_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_16_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_16_buffer->getSize()) + this->gen_03_delay_16_reader - ((size > this->gen_03_delay_16__maxdelay ? this->gen_03_delay_16__maxdelay : (size < (this->gen_03_delay_16_reader != this->gen_03_delay_16_writer) ? this->gen_03_delay_16_reader != this->gen_03_delay_16_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_16_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_16_wrap))
    );
}

void gen_03_delay_16_write(number v) {
    this->gen_03_delay_16_writer = this->gen_03_delay_16_reader;
    this->gen_03_delay_16_buffer[(Index)this->gen_03_delay_16_writer] = v;
}

number gen_03_delay_16_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_16__maxdelay : size);
    number val = this->gen_03_delay_16_read(effectiveSize, 0);
    this->gen_03_delay_16_write(v);
    this->gen_03_delay_16_step();
    return val;
}

array<Index, 2> gen_03_delay_16_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_16_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_16_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_16_init() {
    auto result = this->gen_03_delay_16_calcSizeInSamples();
    this->gen_03_delay_16__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_16_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_16_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_16_clear() {
    this->gen_03_delay_16_buffer->setZero();
}

void gen_03_delay_16_reset() {
    auto result = this->gen_03_delay_16_calcSizeInSamples();
    this->gen_03_delay_16__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_16_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_16_buffer);
    this->gen_03_delay_16_wrap = this->gen_03_delay_16_buffer->getSize() - 1;
    this->gen_03_delay_16_clear();

    if (this->gen_03_delay_16_reader >= this->gen_03_delay_16__maxdelay || this->gen_03_delay_16_writer >= this->gen_03_delay_16__maxdelay) {
        this->gen_03_delay_16_reader = 0;
        this->gen_03_delay_16_writer = 0;
    }
}

void gen_03_delay_16_dspsetup() {
    this->gen_03_delay_16_reset();
}

number gen_03_delay_16_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 12;
}

number gen_03_delay_16_size() {
    return this->gen_03_delay_16__maxdelay;
}

void gen_03_delay_17_step() {
    this->gen_03_delay_17_reader++;

    if (this->gen_03_delay_17_reader >= (int)(this->gen_03_delay_17_buffer->getSize()))
        this->gen_03_delay_17_reader = 0;
}

number gen_03_delay_17_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_03_delay_17_buffer->getSize()) + this->gen_03_delay_17_reader - ((size > this->gen_03_delay_17__maxdelay ? this->gen_03_delay_17__maxdelay : (size < (this->gen_03_delay_17_reader != this->gen_03_delay_17_writer) ? this->gen_03_delay_17_reader != this->gen_03_delay_17_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_17_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_17_wrap))
        ), this->gen_03_delay_17_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_17_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_17_buffer->getSize()) + this->gen_03_delay_17_reader - ((size > this->gen_03_delay_17__maxdelay ? this->gen_03_delay_17__maxdelay : (size < (this->gen_03_delay_17_reader != this->gen_03_delay_17_writer) ? this->gen_03_delay_17_reader != this->gen_03_delay_17_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_17_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_17_wrap))
    );
}

void gen_03_delay_17_write(number v) {
    this->gen_03_delay_17_writer = this->gen_03_delay_17_reader;
    this->gen_03_delay_17_buffer[(Index)this->gen_03_delay_17_writer] = v;
}

number gen_03_delay_17_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_17__maxdelay : size);
    number val = this->gen_03_delay_17_read(effectiveSize, 0);
    this->gen_03_delay_17_write(v);
    this->gen_03_delay_17_step();
    return val;
}

array<Index, 2> gen_03_delay_17_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_17_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_17_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_17_init() {
    auto result = this->gen_03_delay_17_calcSizeInSamples();
    this->gen_03_delay_17__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_17_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_17_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_17_clear() {
    this->gen_03_delay_17_buffer->setZero();
}

void gen_03_delay_17_reset() {
    auto result = this->gen_03_delay_17_calcSizeInSamples();
    this->gen_03_delay_17__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_17_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_17_buffer);
    this->gen_03_delay_17_wrap = this->gen_03_delay_17_buffer->getSize() - 1;
    this->gen_03_delay_17_clear();

    if (this->gen_03_delay_17_reader >= this->gen_03_delay_17__maxdelay || this->gen_03_delay_17_writer >= this->gen_03_delay_17__maxdelay) {
        this->gen_03_delay_17_reader = 0;
        this->gen_03_delay_17_writer = 0;
    }
}

void gen_03_delay_17_dspsetup() {
    this->gen_03_delay_17_reset();
}

number gen_03_delay_17_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 142;
}

number gen_03_delay_17_size() {
    return this->gen_03_delay_17__maxdelay;
}

void gen_03_delay_18_step() {
    this->gen_03_delay_18_reader++;

    if (this->gen_03_delay_18_reader >= (int)(this->gen_03_delay_18_buffer->getSize()))
        this->gen_03_delay_18_reader = 0;
}

number gen_03_delay_18_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_03_delay_18_buffer->getSize()) + this->gen_03_delay_18_reader - ((size > this->gen_03_delay_18__maxdelay ? this->gen_03_delay_18__maxdelay : (size < (this->gen_03_delay_18_reader != this->gen_03_delay_18_writer) ? this->gen_03_delay_18_reader != this->gen_03_delay_18_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_18_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_18_wrap))
        ), this->gen_03_delay_18_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_18_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_18_buffer->getSize()) + this->gen_03_delay_18_reader - ((size > this->gen_03_delay_18__maxdelay ? this->gen_03_delay_18__maxdelay : (size < (this->gen_03_delay_18_reader != this->gen_03_delay_18_writer) ? this->gen_03_delay_18_reader != this->gen_03_delay_18_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_18_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_18_wrap))
    );
}

void gen_03_delay_18_write(number v) {
    this->gen_03_delay_18_writer = this->gen_03_delay_18_reader;
    this->gen_03_delay_18_buffer[(Index)this->gen_03_delay_18_writer] = v;
}

number gen_03_delay_18_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_18__maxdelay : size);
    number val = this->gen_03_delay_18_read(effectiveSize, 0);
    this->gen_03_delay_18_write(v);
    this->gen_03_delay_18_step();
    return val;
}

array<Index, 2> gen_03_delay_18_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_18_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_18_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_18_init() {
    auto result = this->gen_03_delay_18_calcSizeInSamples();
    this->gen_03_delay_18__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_18_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_18_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_18_clear() {
    this->gen_03_delay_18_buffer->setZero();
}

void gen_03_delay_18_reset() {
    auto result = this->gen_03_delay_18_calcSizeInSamples();
    this->gen_03_delay_18__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_18_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_18_buffer);
    this->gen_03_delay_18_wrap = this->gen_03_delay_18_buffer->getSize() - 1;
    this->gen_03_delay_18_clear();

    if (this->gen_03_delay_18_reader >= this->gen_03_delay_18__maxdelay || this->gen_03_delay_18_writer >= this->gen_03_delay_18__maxdelay) {
        this->gen_03_delay_18_reader = 0;
        this->gen_03_delay_18_writer = 0;
    }
}

void gen_03_delay_18_dspsetup() {
    this->gen_03_delay_18_reset();
}

number gen_03_delay_18_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_03_delay_18_size() {
    return this->gen_03_delay_18__maxdelay;
}

number gen_03_history_19_getvalue() {
    return this->gen_03_history_19_value;
}

void gen_03_history_19_setvalue(number val) {
    this->gen_03_history_19_value = val;
}

void gen_03_history_19_reset() {
    this->gen_03_history_19_value = 0;
}

void gen_03_history_19_init() {
    this->gen_03_history_19_value = 0;
}

void gen_03_delay_20_step() {
    this->gen_03_delay_20_reader++;

    if (this->gen_03_delay_20_reader >= (int)(this->gen_03_delay_20_buffer->getSize()))
        this->gen_03_delay_20_reader = 0;
}

number gen_03_delay_20_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_03_delay_20_buffer->getSize()) + this->gen_03_delay_20_reader - ((size > this->gen_03_delay_20__maxdelay ? this->gen_03_delay_20__maxdelay : (size < (this->gen_03_delay_20_reader != this->gen_03_delay_20_writer) ? this->gen_03_delay_20_reader != this->gen_03_delay_20_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_20_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_20_wrap))
        ), this->gen_03_delay_20_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_20_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_20_buffer->getSize()) + this->gen_03_delay_20_reader - ((size > this->gen_03_delay_20__maxdelay ? this->gen_03_delay_20__maxdelay : (size < (this->gen_03_delay_20_reader != this->gen_03_delay_20_writer) ? this->gen_03_delay_20_reader != this->gen_03_delay_20_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_20_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_20_wrap))
    );
}

void gen_03_delay_20_write(number v) {
    this->gen_03_delay_20_writer = this->gen_03_delay_20_reader;
    this->gen_03_delay_20_buffer[(Index)this->gen_03_delay_20_writer] = v;
}

number gen_03_delay_20_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_20__maxdelay : size);
    number val = this->gen_03_delay_20_read(effectiveSize, 0);
    this->gen_03_delay_20_write(v);
    this->gen_03_delay_20_step();
    return val;
}

array<Index, 2> gen_03_delay_20_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_20_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_20_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_20_init() {
    auto result = this->gen_03_delay_20_calcSizeInSamples();
    this->gen_03_delay_20__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_20_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_20_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_20_clear() {
    this->gen_03_delay_20_buffer->setZero();
}

void gen_03_delay_20_reset() {
    auto result = this->gen_03_delay_20_calcSizeInSamples();
    this->gen_03_delay_20__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_20_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_20_buffer);
    this->gen_03_delay_20_wrap = this->gen_03_delay_20_buffer->getSize() - 1;
    this->gen_03_delay_20_clear();

    if (this->gen_03_delay_20_reader >= this->gen_03_delay_20__maxdelay || this->gen_03_delay_20_writer >= this->gen_03_delay_20__maxdelay) {
        this->gen_03_delay_20_reader = 0;
        this->gen_03_delay_20_writer = 0;
    }
}

void gen_03_delay_20_dspsetup() {
    this->gen_03_delay_20_reset();
}

number gen_03_delay_20_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 150;
}

number gen_03_delay_20_size() {
    return this->gen_03_delay_20__maxdelay;
}

void gen_03_delay_21_step() {
    this->gen_03_delay_21_reader++;

    if (this->gen_03_delay_21_reader >= (int)(this->gen_03_delay_21_buffer->getSize()))
        this->gen_03_delay_21_reader = 0;
}

number gen_03_delay_21_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_03_delay_21_buffer->getSize()) + this->gen_03_delay_21_reader - ((size > this->gen_03_delay_21__maxdelay ? this->gen_03_delay_21__maxdelay : (size < (this->gen_03_delay_21_reader != this->gen_03_delay_21_writer) ? this->gen_03_delay_21_reader != this->gen_03_delay_21_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_21_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_21_wrap))
        ), this->gen_03_delay_21_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_21_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_21_buffer->getSize()) + this->gen_03_delay_21_reader - ((size > this->gen_03_delay_21__maxdelay ? this->gen_03_delay_21__maxdelay : (size < (this->gen_03_delay_21_reader != this->gen_03_delay_21_writer) ? this->gen_03_delay_21_reader != this->gen_03_delay_21_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_21_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_21_wrap))
    );
}

void gen_03_delay_21_write(number v) {
    this->gen_03_delay_21_writer = this->gen_03_delay_21_reader;
    this->gen_03_delay_21_buffer[(Index)this->gen_03_delay_21_writer] = v;
}

number gen_03_delay_21_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_21__maxdelay : size);
    number val = this->gen_03_delay_21_read(effectiveSize, 0);
    this->gen_03_delay_21_write(v);
    this->gen_03_delay_21_step();
    return val;
}

array<Index, 2> gen_03_delay_21_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_21_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_21_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_21_init() {
    auto result = this->gen_03_delay_21_calcSizeInSamples();
    this->gen_03_delay_21__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_21_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_21_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_21_clear() {
    this->gen_03_delay_21_buffer->setZero();
}

void gen_03_delay_21_reset() {
    auto result = this->gen_03_delay_21_calcSizeInSamples();
    this->gen_03_delay_21__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_21_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_21_buffer);
    this->gen_03_delay_21_wrap = this->gen_03_delay_21_buffer->getSize() - 1;
    this->gen_03_delay_21_clear();

    if (this->gen_03_delay_21_reader >= this->gen_03_delay_21__maxdelay || this->gen_03_delay_21_writer >= this->gen_03_delay_21__maxdelay) {
        this->gen_03_delay_21_reader = 0;
        this->gen_03_delay_21_writer = 0;
    }
}

void gen_03_delay_21_dspsetup() {
    this->gen_03_delay_21_reset();
}

number gen_03_delay_21_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_03_delay_21_size() {
    return this->gen_03_delay_21__maxdelay;
}

number gen_03_history_22_getvalue() {
    return this->gen_03_history_22_value;
}

void gen_03_history_22_setvalue(number val) {
    this->gen_03_history_22_value = val;
}

void gen_03_history_22_reset() {
    this->gen_03_history_22_value = 0;
}

void gen_03_history_22_init() {
    this->gen_03_history_22_value = 0;
}

void gen_03_delay_23_step() {
    this->gen_03_delay_23_reader++;

    if (this->gen_03_delay_23_reader >= (int)(this->gen_03_delay_23_buffer->getSize()))
        this->gen_03_delay_23_reader = 0;
}

number gen_03_delay_23_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_03_delay_23_buffer->getSize()) + this->gen_03_delay_23_reader - ((size > this->gen_03_delay_23__maxdelay ? this->gen_03_delay_23__maxdelay : (size < (this->gen_03_delay_23_reader != this->gen_03_delay_23_writer) ? this->gen_03_delay_23_reader != this->gen_03_delay_23_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_23_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_23_wrap))
        ), this->gen_03_delay_23_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_23_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_23_buffer->getSize()) + this->gen_03_delay_23_reader - ((size > this->gen_03_delay_23__maxdelay ? this->gen_03_delay_23__maxdelay : (size < (this->gen_03_delay_23_reader != this->gen_03_delay_23_writer) ? this->gen_03_delay_23_reader != this->gen_03_delay_23_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_23_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_23_wrap))
    );
}

void gen_03_delay_23_write(number v) {
    this->gen_03_delay_23_writer = this->gen_03_delay_23_reader;
    this->gen_03_delay_23_buffer[(Index)this->gen_03_delay_23_writer] = v;
}

number gen_03_delay_23_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_23__maxdelay : size);
    number val = this->gen_03_delay_23_read(effectiveSize, 0);
    this->gen_03_delay_23_write(v);
    this->gen_03_delay_23_step();
    return val;
}

array<Index, 2> gen_03_delay_23_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_23_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_23_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_23_init() {
    auto result = this->gen_03_delay_23_calcSizeInSamples();
    this->gen_03_delay_23__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_23_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_23_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_23_clear() {
    this->gen_03_delay_23_buffer->setZero();
}

void gen_03_delay_23_reset() {
    auto result = this->gen_03_delay_23_calcSizeInSamples();
    this->gen_03_delay_23__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_23_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_23_buffer);
    this->gen_03_delay_23_wrap = this->gen_03_delay_23_buffer->getSize() - 1;
    this->gen_03_delay_23_clear();

    if (this->gen_03_delay_23_reader >= this->gen_03_delay_23__maxdelay || this->gen_03_delay_23_writer >= this->gen_03_delay_23__maxdelay) {
        this->gen_03_delay_23_reader = 0;
        this->gen_03_delay_23_writer = 0;
    }
}

void gen_03_delay_23_dspsetup() {
    this->gen_03_delay_23_reset();
}

number gen_03_delay_23_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 2000;
}

number gen_03_delay_23_size() {
    return this->gen_03_delay_23__maxdelay;
}

void gen_03_delay_24_step() {
    this->gen_03_delay_24_reader++;

    if (this->gen_03_delay_24_reader >= (int)(this->gen_03_delay_24_buffer->getSize()))
        this->gen_03_delay_24_reader = 0;
}

number gen_03_delay_24_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_03_delay_24_buffer->getSize()) + this->gen_03_delay_24_reader - ((size > this->gen_03_delay_24__maxdelay ? this->gen_03_delay_24__maxdelay : (size < (this->gen_03_delay_24_reader != this->gen_03_delay_24_writer) ? this->gen_03_delay_24_reader != this->gen_03_delay_24_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_24_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_24_wrap))
        ), this->gen_03_delay_24_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_24_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_24_buffer->getSize()) + this->gen_03_delay_24_reader - ((size > this->gen_03_delay_24__maxdelay ? this->gen_03_delay_24__maxdelay : (size < (this->gen_03_delay_24_reader != this->gen_03_delay_24_writer) ? this->gen_03_delay_24_reader != this->gen_03_delay_24_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_24_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_24_wrap))
    );
}

void gen_03_delay_24_write(number v) {
    this->gen_03_delay_24_writer = this->gen_03_delay_24_reader;
    this->gen_03_delay_24_buffer[(Index)this->gen_03_delay_24_writer] = v;
}

number gen_03_delay_24_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_24__maxdelay : size);
    number val = this->gen_03_delay_24_read(effectiveSize, 0);
    this->gen_03_delay_24_write(v);
    this->gen_03_delay_24_step();
    return val;
}

array<Index, 2> gen_03_delay_24_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_24_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_24_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_24_init() {
    auto result = this->gen_03_delay_24_calcSizeInSamples();
    this->gen_03_delay_24__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_24_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_24_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_24_clear() {
    this->gen_03_delay_24_buffer->setZero();
}

void gen_03_delay_24_reset() {
    auto result = this->gen_03_delay_24_calcSizeInSamples();
    this->gen_03_delay_24__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_24_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_24_buffer);
    this->gen_03_delay_24_wrap = this->gen_03_delay_24_buffer->getSize() - 1;
    this->gen_03_delay_24_clear();

    if (this->gen_03_delay_24_reader >= this->gen_03_delay_24__maxdelay || this->gen_03_delay_24_writer >= this->gen_03_delay_24__maxdelay) {
        this->gen_03_delay_24_reader = 0;
        this->gen_03_delay_24_writer = 0;
    }
}

void gen_03_delay_24_dspsetup() {
    this->gen_03_delay_24_reset();
}

number gen_03_delay_24_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_03_delay_24_size() {
    return this->gen_03_delay_24__maxdelay;
}

number gen_03_history_25_getvalue() {
    return this->gen_03_history_25_value;
}

void gen_03_history_25_setvalue(number val) {
    this->gen_03_history_25_value = val;
}

void gen_03_history_25_reset() {
    this->gen_03_history_25_value = 0;
}

void gen_03_history_25_init() {
    this->gen_03_history_25_value = 0;
}

void gen_03_delay_26_step() {
    this->gen_03_delay_26_reader++;

    if (this->gen_03_delay_26_reader >= (int)(this->gen_03_delay_26_buffer->getSize()))
        this->gen_03_delay_26_reader = 0;
}

number gen_03_delay_26_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_03_delay_26_buffer->getSize()) + this->gen_03_delay_26_reader - ((size > this->gen_03_delay_26__maxdelay ? this->gen_03_delay_26__maxdelay : (size < (this->gen_03_delay_26_reader != this->gen_03_delay_26_writer) ? this->gen_03_delay_26_reader != this->gen_03_delay_26_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_26_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_26_wrap))
        ), this->gen_03_delay_26_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_26_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_26_buffer->getSize()) + this->gen_03_delay_26_reader - ((size > this->gen_03_delay_26__maxdelay ? this->gen_03_delay_26__maxdelay : (size < (this->gen_03_delay_26_reader != this->gen_03_delay_26_writer) ? this->gen_03_delay_26_reader != this->gen_03_delay_26_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_26_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_26_wrap))
    );
}

void gen_03_delay_26_write(number v) {
    this->gen_03_delay_26_writer = this->gen_03_delay_26_reader;
    this->gen_03_delay_26_buffer[(Index)this->gen_03_delay_26_writer] = v;
}

number gen_03_delay_26_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_26__maxdelay : size);
    number val = this->gen_03_delay_26_read(effectiveSize, 0);
    this->gen_03_delay_26_write(v);
    this->gen_03_delay_26_step();
    return val;
}

array<Index, 2> gen_03_delay_26_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_26_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_26_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_26_init() {
    auto result = this->gen_03_delay_26_calcSizeInSamples();
    this->gen_03_delay_26__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_26_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_26_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_26_clear() {
    this->gen_03_delay_26_buffer->setZero();
}

void gen_03_delay_26_reset() {
    auto result = this->gen_03_delay_26_calcSizeInSamples();
    this->gen_03_delay_26__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_26_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_26_buffer);
    this->gen_03_delay_26_wrap = this->gen_03_delay_26_buffer->getSize() - 1;
    this->gen_03_delay_26_clear();

    if (this->gen_03_delay_26_reader >= this->gen_03_delay_26__maxdelay || this->gen_03_delay_26_writer >= this->gen_03_delay_26__maxdelay) {
        this->gen_03_delay_26_reader = 0;
        this->gen_03_delay_26_writer = 0;
    }
}

void gen_03_delay_26_dspsetup() {
    this->gen_03_delay_26_reset();
}

number gen_03_delay_26_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_03_delay_26_size() {
    return this->gen_03_delay_26__maxdelay;
}

number gen_03_history_27_getvalue() {
    return this->gen_03_history_27_value;
}

void gen_03_history_27_setvalue(number val) {
    this->gen_03_history_27_value = val;
}

void gen_03_history_27_reset() {
    this->gen_03_history_27_value = 0;
}

void gen_03_history_27_init() {
    this->gen_03_history_27_value = 0;
}

void gen_03_delay_28_step() {
    this->gen_03_delay_28_reader++;

    if (this->gen_03_delay_28_reader >= (int)(this->gen_03_delay_28_buffer->getSize()))
        this->gen_03_delay_28_reader = 0;
}

number gen_03_delay_28_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_03_delay_28_buffer->getSize()) + this->gen_03_delay_28_reader - ((size > this->gen_03_delay_28__maxdelay ? this->gen_03_delay_28__maxdelay : (size < (this->gen_03_delay_28_reader != this->gen_03_delay_28_writer) ? this->gen_03_delay_28_reader != this->gen_03_delay_28_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_28_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_28_wrap))
        ), this->gen_03_delay_28_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_28_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_28_buffer->getSize()) + this->gen_03_delay_28_reader - ((size > this->gen_03_delay_28__maxdelay ? this->gen_03_delay_28__maxdelay : (size < (this->gen_03_delay_28_reader != this->gen_03_delay_28_writer) ? this->gen_03_delay_28_reader != this->gen_03_delay_28_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_28_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_28_wrap))
    );
}

void gen_03_delay_28_write(number v) {
    this->gen_03_delay_28_writer = this->gen_03_delay_28_reader;
    this->gen_03_delay_28_buffer[(Index)this->gen_03_delay_28_writer] = v;
}

number gen_03_delay_28_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_28__maxdelay : size);
    number val = this->gen_03_delay_28_read(effectiveSize, 0);
    this->gen_03_delay_28_write(v);
    this->gen_03_delay_28_step();
    return val;
}

array<Index, 2> gen_03_delay_28_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_28_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_28_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_28_init() {
    auto result = this->gen_03_delay_28_calcSizeInSamples();
    this->gen_03_delay_28__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_28_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_28_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_28_clear() {
    this->gen_03_delay_28_buffer->setZero();
}

void gen_03_delay_28_reset() {
    auto result = this->gen_03_delay_28_calcSizeInSamples();
    this->gen_03_delay_28__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_28_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_28_buffer);
    this->gen_03_delay_28_wrap = this->gen_03_delay_28_buffer->getSize() - 1;
    this->gen_03_delay_28_clear();

    if (this->gen_03_delay_28_reader >= this->gen_03_delay_28__maxdelay || this->gen_03_delay_28_writer >= this->gen_03_delay_28__maxdelay) {
        this->gen_03_delay_28_reader = 0;
        this->gen_03_delay_28_writer = 0;
    }
}

void gen_03_delay_28_dspsetup() {
    this->gen_03_delay_28_reset();
}

number gen_03_delay_28_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_03_delay_28_size() {
    return this->gen_03_delay_28__maxdelay;
}

number gen_03_history_29_getvalue() {
    return this->gen_03_history_29_value;
}

void gen_03_history_29_setvalue(number val) {
    this->gen_03_history_29_value = val;
}

void gen_03_history_29_reset() {
    this->gen_03_history_29_value = 0;
}

void gen_03_history_29_init() {
    this->gen_03_history_29_value = 0;
}

void gen_03_delay_30_step() {
    this->gen_03_delay_30_reader++;

    if (this->gen_03_delay_30_reader >= (int)(this->gen_03_delay_30_buffer->getSize()))
        this->gen_03_delay_30_reader = 0;
}

number gen_03_delay_30_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_03_delay_30_buffer->getSize()) + this->gen_03_delay_30_reader - ((size > this->gen_03_delay_30__maxdelay ? this->gen_03_delay_30__maxdelay : (size < (this->gen_03_delay_30_reader != this->gen_03_delay_30_writer) ? this->gen_03_delay_30_reader != this->gen_03_delay_30_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_03_delay_30_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_30_wrap))
        ), this->gen_03_delay_30_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_03_delay_30_wrap))
        ));
    }

    number r = (int)(this->gen_03_delay_30_buffer->getSize()) + this->gen_03_delay_30_reader - ((size > this->gen_03_delay_30__maxdelay ? this->gen_03_delay_30__maxdelay : (size < (this->gen_03_delay_30_reader != this->gen_03_delay_30_writer) ? this->gen_03_delay_30_reader != this->gen_03_delay_30_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_03_delay_30_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_03_delay_30_wrap))
    );
}

void gen_03_delay_30_write(number v) {
    this->gen_03_delay_30_writer = this->gen_03_delay_30_reader;
    this->gen_03_delay_30_buffer[(Index)this->gen_03_delay_30_writer] = v;
}

number gen_03_delay_30_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_03_delay_30__maxdelay : size);
    number val = this->gen_03_delay_30_read(effectiveSize, 0);
    this->gen_03_delay_30_write(v);
    this->gen_03_delay_30_step();
    return val;
}

array<Index, 2> gen_03_delay_30_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_03_delay_30_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_03_delay_30_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_03_delay_30_init() {
    auto result = this->gen_03_delay_30_calcSizeInSamples();
    this->gen_03_delay_30__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_30_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_03_delay_30_wrap = requestedSizeInSamples - 1;
}

void gen_03_delay_30_clear() {
    this->gen_03_delay_30_buffer->setZero();
}

void gen_03_delay_30_reset() {
    auto result = this->gen_03_delay_30_calcSizeInSamples();
    this->gen_03_delay_30__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_03_delay_30_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_03_delay_30_buffer);
    this->gen_03_delay_30_wrap = this->gen_03_delay_30_buffer->getSize() - 1;
    this->gen_03_delay_30_clear();

    if (this->gen_03_delay_30_reader >= this->gen_03_delay_30__maxdelay || this->gen_03_delay_30_writer >= this->gen_03_delay_30__maxdelay) {
        this->gen_03_delay_30_reader = 0;
        this->gen_03_delay_30_writer = 0;
    }
}

void gen_03_delay_30_dspsetup() {
    this->gen_03_delay_30_reset();
}

number gen_03_delay_30_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_03_delay_30_size() {
    return this->gen_03_delay_30__maxdelay;
}

number gen_03_history_31_getvalue() {
    return this->gen_03_history_31_value;
}

void gen_03_history_31_setvalue(number val) {
    this->gen_03_history_31_value = val;
}

void gen_03_history_31_reset() {
    this->gen_03_history_31_value = 0;
}

void gen_03_history_31_init() {
    this->gen_03_history_31_value = 0;
}

number gen_03_history_32_getvalue() {
    return this->gen_03_history_32_value;
}

void gen_03_history_32_setvalue(number val) {
    this->gen_03_history_32_value = val;
}

void gen_03_history_32_reset() {
    this->gen_03_history_32_value = 0;
}

void gen_03_history_32_init() {
    this->gen_03_history_32_value = 0;
}

number gen_03_history_33_getvalue() {
    return this->gen_03_history_33_value;
}

void gen_03_history_33_setvalue(number val) {
    this->gen_03_history_33_value = val;
}

void gen_03_history_33_reset() {
    this->gen_03_history_33_value = 0;
}

void gen_03_history_33_init() {
    this->gen_03_history_33_value = 0;
}

number gen_03_cycle_39_ph_next(number freq, number reset) {
    RNBO_UNUSED(reset);

    {
        {}
    }

    number pincr = freq * this->gen_03_cycle_39_ph_conv;

    if (this->gen_03_cycle_39_ph_currentPhase < 0.)
        this->gen_03_cycle_39_ph_currentPhase = 1. + this->gen_03_cycle_39_ph_currentPhase;

    if (this->gen_03_cycle_39_ph_currentPhase > 1.)
        this->gen_03_cycle_39_ph_currentPhase = this->gen_03_cycle_39_ph_currentPhase - 1.;

    number tmp = this->gen_03_cycle_39_ph_currentPhase;
    this->gen_03_cycle_39_ph_currentPhase += pincr;
    return tmp;
}

void gen_03_cycle_39_ph_reset() {
    this->gen_03_cycle_39_ph_currentPhase = 0;
}

void gen_03_cycle_39_ph_dspsetup() {
    this->gen_03_cycle_39_ph_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

array<number, 2> gen_03_cycle_39_next(number frequency, number phase_offset) {
    RNBO_UNUSED(phase_offset);
    RNBO_UNUSED(frequency);

    {
        uint32_t uint_phase;

        {
            uint_phase = this->gen_03_cycle_39_phasei;
        }

        uint32_t idx = (uint32_t)(uint32_rshift(uint_phase, 18));
        number frac = ((BinOpInt)((BinOpInt)uint_phase & (BinOpInt)262143)) * 3.81471181759574e-6;
        number y0 = this->gen_03_cycle_39_buffer[(Index)idx];
        number y1 = this->gen_03_cycle_39_buffer[(Index)((BinOpInt)(idx + 1) & (BinOpInt)16383)];
        number y = y0 + frac * (y1 - y0);

        {
            uint32_t pincr = (uint32_t)(uint32_trunc(0.1 * this->gen_03_cycle_39_f2i));
            this->gen_03_cycle_39_phasei = uint32_add(this->gen_03_cycle_39_phasei, pincr);
        }

        return {y, uint_phase * 0.232830643653869629e-9};
    }
}

void gen_03_cycle_39_dspsetup() {
    this->gen_03_cycle_39_phasei = 0;
    this->gen_03_cycle_39_f2i = (this->samplerate() == 0. ? 0. : (number)4294967296 / this->samplerate());
    this->gen_03_cycle_39_wrap = (long)(this->gen_03_cycle_39_buffer->getSize()) - 1;
}

void gen_03_cycle_39_reset() {
    this->gen_03_cycle_39_phasei = 0;
}

void gen_03_cycle_39_bufferUpdated() {
    this->gen_03_cycle_39_wrap = (long)(this->gen_03_cycle_39_buffer->getSize()) - 1;
}

number gen_03_cycle_54_ph_next(number freq, number reset) {
    RNBO_UNUSED(reset);

    {
        {}
    }

    number pincr = freq * this->gen_03_cycle_54_ph_conv;

    if (this->gen_03_cycle_54_ph_currentPhase < 0.)
        this->gen_03_cycle_54_ph_currentPhase = 1. + this->gen_03_cycle_54_ph_currentPhase;

    if (this->gen_03_cycle_54_ph_currentPhase > 1.)
        this->gen_03_cycle_54_ph_currentPhase = this->gen_03_cycle_54_ph_currentPhase - 1.;

    number tmp = this->gen_03_cycle_54_ph_currentPhase;
    this->gen_03_cycle_54_ph_currentPhase += pincr;
    return tmp;
}

void gen_03_cycle_54_ph_reset() {
    this->gen_03_cycle_54_ph_currentPhase = 0;
}

void gen_03_cycle_54_ph_dspsetup() {
    this->gen_03_cycle_54_ph_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

array<number, 2> gen_03_cycle_54_next(number frequency, number phase_offset) {
    RNBO_UNUSED(phase_offset);
    RNBO_UNUSED(frequency);

    {
        uint32_t uint_phase;

        {
            uint_phase = this->gen_03_cycle_54_phasei;
        }

        uint32_t idx = (uint32_t)(uint32_rshift(uint_phase, 18));
        number frac = ((BinOpInt)((BinOpInt)uint_phase & (BinOpInt)262143)) * 3.81471181759574e-6;
        number y0 = this->gen_03_cycle_54_buffer[(Index)idx];
        number y1 = this->gen_03_cycle_54_buffer[(Index)((BinOpInt)(idx + 1) & (BinOpInt)16383)];
        number y = y0 + frac * (y1 - y0);

        {
            uint32_t pincr = (uint32_t)(uint32_trunc(0.1 * this->gen_03_cycle_54_f2i));
            this->gen_03_cycle_54_phasei = uint32_add(this->gen_03_cycle_54_phasei, pincr);
        }

        return {y, uint_phase * 0.232830643653869629e-9};
    }
}

void gen_03_cycle_54_dspsetup() {
    this->gen_03_cycle_54_phasei = 0;
    this->gen_03_cycle_54_f2i = (this->samplerate() == 0. ? 0. : (number)4294967296 / this->samplerate());
    this->gen_03_cycle_54_wrap = (long)(this->gen_03_cycle_54_buffer->getSize()) - 1;
}

void gen_03_cycle_54_reset() {
    this->gen_03_cycle_54_phasei = 0;
}

void gen_03_cycle_54_bufferUpdated() {
    this->gen_03_cycle_54_wrap = (long)(this->gen_03_cycle_54_buffer->getSize()) - 1;
}

void gen_03_dspsetup(bool force) {
    if ((bool)(this->gen_03_setupDone) && (bool)(!(bool)(force)))
        return;

    this->gen_03_setupDone = true;
    this->gen_03_delay_1_dspsetup();
    this->gen_03_delay_2_dspsetup();
    this->gen_03_delay_3_dspsetup();
    this->gen_03_delay_4_dspsetup();
    this->gen_03_delay_5_dspsetup();
    this->gen_03_delay_6_dspsetup();
    this->gen_03_delay_7_dspsetup();
    this->gen_03_delay_8_dspsetup();
    this->gen_03_delay_9_dspsetup();
    this->gen_03_delay_10_dspsetup();
    this->gen_03_delay_11_dspsetup();
    this->gen_03_delay_12_dspsetup();
    this->gen_03_delay_13_dspsetup();
    this->gen_03_delay_14_dspsetup();
    this->gen_03_delay_15_dspsetup();
    this->gen_03_delay_16_dspsetup();
    this->gen_03_delay_17_dspsetup();
    this->gen_03_delay_18_dspsetup();
    this->gen_03_delay_20_dspsetup();
    this->gen_03_delay_21_dspsetup();
    this->gen_03_delay_23_dspsetup();
    this->gen_03_delay_24_dspsetup();
    this->gen_03_delay_26_dspsetup();
    this->gen_03_delay_28_dspsetup();
    this->gen_03_delay_30_dspsetup();
    this->gen_03_cycle_39_ph_dspsetup();
    this->gen_03_cycle_39_dspsetup();
    this->gen_03_cycle_54_ph_dspsetup();
    this->gen_03_cycle_54_dspsetup();
}

number gen_04_history_1_getvalue() {
    return this->gen_04_history_1_value;
}

void gen_04_history_1_setvalue(number val) {
    this->gen_04_history_1_value = val;
}

void gen_04_history_1_reset() {
    this->gen_04_history_1_value = 0;
}

void gen_04_history_1_init() {
    this->gen_04_history_1_value = 0;
}

number gen_04_history_2_getvalue() {
    return this->gen_04_history_2_value;
}

void gen_04_history_2_setvalue(number val) {
    this->gen_04_history_2_value = val;
}

void gen_04_history_2_reset() {
    this->gen_04_history_2_value = 0;
}

void gen_04_history_2_init() {
    this->gen_04_history_2_value = 0;
}

void gen_04_delay_3_step() {
    this->gen_04_delay_3_reader++;

    if (this->gen_04_delay_3_reader >= (int)(this->gen_04_delay_3_buffer->getSize()))
        this->gen_04_delay_3_reader = 0;
}

number gen_04_delay_3_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_04_delay_3_buffer->getSize()) + this->gen_04_delay_3_reader - ((size > this->gen_04_delay_3__maxdelay ? this->gen_04_delay_3__maxdelay : (size < (this->gen_04_delay_3_reader != this->gen_04_delay_3_writer) ? this->gen_04_delay_3_reader != this->gen_04_delay_3_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_04_delay_3_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_04_delay_3_wrap))
        ), this->gen_04_delay_3_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_04_delay_3_wrap))
        ));
    }

    number r = (int)(this->gen_04_delay_3_buffer->getSize()) + this->gen_04_delay_3_reader - ((size > this->gen_04_delay_3__maxdelay ? this->gen_04_delay_3__maxdelay : (size < (this->gen_04_delay_3_reader != this->gen_04_delay_3_writer) ? this->gen_04_delay_3_reader != this->gen_04_delay_3_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_04_delay_3_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_04_delay_3_wrap))
    );
}

void gen_04_delay_3_write(number v) {
    this->gen_04_delay_3_writer = this->gen_04_delay_3_reader;
    this->gen_04_delay_3_buffer[(Index)this->gen_04_delay_3_writer] = v;
}

number gen_04_delay_3_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_04_delay_3__maxdelay : size);
    number val = this->gen_04_delay_3_read(effectiveSize, 0);
    this->gen_04_delay_3_write(v);
    this->gen_04_delay_3_step();
    return val;
}

array<Index, 2> gen_04_delay_3_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_04_delay_3_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_04_delay_3_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_04_delay_3_init() {
    auto result = this->gen_04_delay_3_calcSizeInSamples();
    this->gen_04_delay_3__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_04_delay_3_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_04_delay_3_wrap = requestedSizeInSamples - 1;
}

void gen_04_delay_3_clear() {
    this->gen_04_delay_3_buffer->setZero();
}

void gen_04_delay_3_reset() {
    auto result = this->gen_04_delay_3_calcSizeInSamples();
    this->gen_04_delay_3__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_04_delay_3_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_04_delay_3_buffer);
    this->gen_04_delay_3_wrap = this->gen_04_delay_3_buffer->getSize() - 1;
    this->gen_04_delay_3_clear();

    if (this->gen_04_delay_3_reader >= this->gen_04_delay_3__maxdelay || this->gen_04_delay_3_writer >= this->gen_04_delay_3__maxdelay) {
        this->gen_04_delay_3_reader = 0;
        this->gen_04_delay_3_writer = 0;
    }
}

void gen_04_delay_3_dspsetup() {
    this->gen_04_delay_3_reset();
}

number gen_04_delay_3_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_04_delay_3_size() {
    return this->gen_04_delay_3__maxdelay;
}

void gen_04_delay_4_step() {
    this->gen_04_delay_4_reader++;

    if (this->gen_04_delay_4_reader >= (int)(this->gen_04_delay_4_buffer->getSize()))
        this->gen_04_delay_4_reader = 0;
}

number gen_04_delay_4_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_04_delay_4_buffer->getSize()) + this->gen_04_delay_4_reader - ((size > this->gen_04_delay_4__maxdelay ? this->gen_04_delay_4__maxdelay : (size < (this->gen_04_delay_4_reader != this->gen_04_delay_4_writer) ? this->gen_04_delay_4_reader != this->gen_04_delay_4_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_04_delay_4_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_04_delay_4_wrap))
        ), this->gen_04_delay_4_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_04_delay_4_wrap))
        ));
    }

    number r = (int)(this->gen_04_delay_4_buffer->getSize()) + this->gen_04_delay_4_reader - ((size > this->gen_04_delay_4__maxdelay ? this->gen_04_delay_4__maxdelay : (size < (this->gen_04_delay_4_reader != this->gen_04_delay_4_writer) ? this->gen_04_delay_4_reader != this->gen_04_delay_4_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_04_delay_4_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_04_delay_4_wrap))
    );
}

void gen_04_delay_4_write(number v) {
    this->gen_04_delay_4_writer = this->gen_04_delay_4_reader;
    this->gen_04_delay_4_buffer[(Index)this->gen_04_delay_4_writer] = v;
}

number gen_04_delay_4_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_04_delay_4__maxdelay : size);
    number val = this->gen_04_delay_4_read(effectiveSize, 0);
    this->gen_04_delay_4_write(v);
    this->gen_04_delay_4_step();
    return val;
}

array<Index, 2> gen_04_delay_4_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_04_delay_4_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_04_delay_4_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_04_delay_4_init() {
    auto result = this->gen_04_delay_4_calcSizeInSamples();
    this->gen_04_delay_4__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_04_delay_4_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_04_delay_4_wrap = requestedSizeInSamples - 1;
}

void gen_04_delay_4_clear() {
    this->gen_04_delay_4_buffer->setZero();
}

void gen_04_delay_4_reset() {
    auto result = this->gen_04_delay_4_calcSizeInSamples();
    this->gen_04_delay_4__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_04_delay_4_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_04_delay_4_buffer);
    this->gen_04_delay_4_wrap = this->gen_04_delay_4_buffer->getSize() - 1;
    this->gen_04_delay_4_clear();

    if (this->gen_04_delay_4_reader >= this->gen_04_delay_4__maxdelay || this->gen_04_delay_4_writer >= this->gen_04_delay_4__maxdelay) {
        this->gen_04_delay_4_reader = 0;
        this->gen_04_delay_4_writer = 0;
    }
}

void gen_04_delay_4_dspsetup() {
    this->gen_04_delay_4_reset();
}

number gen_04_delay_4_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_04_delay_4_size() {
    return this->gen_04_delay_4__maxdelay;
}

number gen_04_history_5_getvalue() {
    return this->gen_04_history_5_value;
}

void gen_04_history_5_setvalue(number val) {
    this->gen_04_history_5_value = val;
}

void gen_04_history_5_reset() {
    this->gen_04_history_5_value = 0;
}

void gen_04_history_5_init() {
    this->gen_04_history_5_value = 0;
}

number gen_04_history_6_getvalue() {
    return this->gen_04_history_6_value;
}

void gen_04_history_6_setvalue(number val) {
    this->gen_04_history_6_value = val;
}

void gen_04_history_6_reset() {
    this->gen_04_history_6_value = 0;
}

void gen_04_history_6_init() {
    this->gen_04_history_6_value = 0;
}

number gen_04_history_7_getvalue() {
    return this->gen_04_history_7_value;
}

void gen_04_history_7_setvalue(number val) {
    this->gen_04_history_7_value = val;
}

void gen_04_history_7_reset() {
    this->gen_04_history_7_value = 0;
}

void gen_04_history_7_init() {
    this->gen_04_history_7_value = 0;
}

number gen_04_history_8_getvalue() {
    return this->gen_04_history_8_value;
}

void gen_04_history_8_setvalue(number val) {
    this->gen_04_history_8_value = val;
}

void gen_04_history_8_reset() {
    this->gen_04_history_8_value = 0;
}

void gen_04_history_8_init() {
    this->gen_04_history_8_value = 0;
}

number gen_04_history_9_getvalue() {
    return this->gen_04_history_9_value;
}

void gen_04_history_9_setvalue(number val) {
    this->gen_04_history_9_value = val;
}

void gen_04_history_9_reset() {
    this->gen_04_history_9_value = 0;
}

void gen_04_history_9_init() {
    this->gen_04_history_9_value = 0;
}

number gen_04_history_10_getvalue() {
    return this->gen_04_history_10_value;
}

void gen_04_history_10_setvalue(number val) {
    this->gen_04_history_10_value = val;
}

void gen_04_history_10_reset() {
    this->gen_04_history_10_value = 0;
}

void gen_04_history_10_init() {
    this->gen_04_history_10_value = 0;
}

number gen_04_history_11_getvalue() {
    return this->gen_04_history_11_value;
}

void gen_04_history_11_setvalue(number val) {
    this->gen_04_history_11_value = val;
}

void gen_04_history_11_reset() {
    this->gen_04_history_11_value = 0;
}

void gen_04_history_11_init() {
    this->gen_04_history_11_value = 0;
}

number gen_04_history_12_getvalue() {
    return this->gen_04_history_12_value;
}

void gen_04_history_12_setvalue(number val) {
    this->gen_04_history_12_value = val;
}

void gen_04_history_12_reset() {
    this->gen_04_history_12_value = 0;
}

void gen_04_history_12_init() {
    this->gen_04_history_12_value = 0;
}

void gen_04_dspsetup(bool force) {
    if ((bool)(this->gen_04_setupDone) && (bool)(!(bool)(force)))
        return;

    this->gen_04_setupDone = true;
    this->gen_04_delay_3_dspsetup();
    this->gen_04_delay_4_dspsetup();
}

void numberobj_10_init() {
    this->numberobj_10_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-55"), 1, this->_currentTime);
}

void numberobj_10_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_10_value;
}

void numberobj_10_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_10_value_set(preset["value"]);
}

void numberobj_11_init() {
    this->numberobj_11_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-16"), 1, this->_currentTime);
}

void numberobj_11_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_11_value;
}

void numberobj_11_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_11_value_set(preset["value"]);
}

void numberobj_12_init() {
    this->numberobj_12_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-79"), 1, this->_currentTime);
}

void numberobj_12_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_12_value;
}

void numberobj_12_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_12_value_set(preset["value"]);
}

void numberobj_13_init() {
    this->numberobj_13_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-78"), 1, this->_currentTime);
}

void numberobj_13_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_13_value;
}

void numberobj_13_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_13_value_set(preset["value"]);
}

void numberobj_14_init() {
    this->numberobj_14_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-60"), 1, this->_currentTime);
}

void numberobj_14_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_14_value;
}

void numberobj_14_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_14_value_set(preset["value"]);
}

void numberobj_15_init() {
    this->numberobj_15_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-73"), 1, this->_currentTime);
}

void numberobj_15_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_15_value;
}

void numberobj_15_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_15_value_set(preset["value"]);
}

void numberobj_16_init() {
    this->numberobj_16_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-72"), 1, this->_currentTime);
}

void numberobj_16_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_16_value;
}

void numberobj_16_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_16_value_set(preset["value"]);
}

void numberobj_17_init() {
    this->numberobj_17_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-69"), 1, this->_currentTime);
}

void numberobj_17_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_17_value;
}

void numberobj_17_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_17_value_set(preset["value"]);
}

void metertilde_01_init() {
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("meter~_obj-100"), 1, this->_currentTime);
}

void metertilde_01_dspsetup(bool force) {
    if ((bool)(this->metertilde_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->metertilde_01_currentIntervalInSamples = this->mstosamps(50);
    this->metertilde_01_currentInterval = this->metertilde_01_currentIntervalInSamples;
    this->metertilde_01_setupDone = true;
}

void numberobj_18_init() {
    this->numberobj_18_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-80"), 1, this->_currentTime);
}

void numberobj_18_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_18_value;
}

void numberobj_18_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_18_value_set(preset["value"]);
}

void numberobj_19_init() {
    this->numberobj_19_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-68"), 1, this->_currentTime);
}

void numberobj_19_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_19_value;
}

void numberobj_19_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_19_value_set(preset["value"]);
}

void numberobj_20_init() {
    this->numberobj_20_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-38"), 1, this->_currentTime);
}

void numberobj_20_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_20_value;
}

void numberobj_20_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_20_value_set(preset["value"]);
}

void numberobj_21_init() {
    this->numberobj_21_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-97"), 1, this->_currentTime);
}

void numberobj_21_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_21_value;
}

void numberobj_21_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_21_value_set(preset["value"]);
}

void numberobj_22_init() {
    this->numberobj_22_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-163"), 1, this->_currentTime);
}

void numberobj_22_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_22_value;
}

void numberobj_22_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_22_value_set(preset["value"]);
}

void param_03_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_03_value;
}

void param_03_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_03_value_set(preset["value"]);
}

void param_04_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_04_value;
}

void param_04_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_04_value_set(preset["value"]);
}

void slider_01_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->slider_01_value;
}

void slider_01_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->slider_01_value_set(preset["value"]);
}

void numberobj_23_init() {
    this->numberobj_23_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-142"), 1, this->_currentTime);
}

void numberobj_23_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_23_value;
}

void numberobj_23_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_23_value_set(preset["value"]);
}

void param_05_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_05_value;
}

void param_05_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_05_value_set(preset["value"]);
}

void param_06_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_06_value;
}

void param_06_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_06_value_set(preset["value"]);
}

void numberobj_24_init() {
    this->numberobj_24_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-20"), 1, this->_currentTime);
}

void numberobj_24_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_24_value;
}

void numberobj_24_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_24_value_set(preset["value"]);
}

void param_07_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_07_value;
}

void param_07_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_07_value_set(preset["value"]);
}

void numberobj_25_init() {
    this->numberobj_25_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-24"), 1, this->_currentTime);
}

void numberobj_25_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_25_value;
}

void numberobj_25_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_25_value_set(preset["value"]);
}

number gen_05_history_1_getvalue() {
    return this->gen_05_history_1_value;
}

void gen_05_history_1_setvalue(number val) {
    this->gen_05_history_1_value = val;
}

void gen_05_history_1_reset() {
    this->gen_05_history_1_value = 0;
}

void gen_05_history_1_init() {
    this->gen_05_history_1_value = 0;
}

number gen_05_history_2_getvalue() {
    return this->gen_05_history_2_value;
}

void gen_05_history_2_setvalue(number val) {
    this->gen_05_history_2_value = val;
}

void gen_05_history_2_reset() {
    this->gen_05_history_2_value = 0;
}

void gen_05_history_2_init() {
    this->gen_05_history_2_value = 0;
}

void gen_05_delay_3_step() {
    this->gen_05_delay_3_reader++;

    if (this->gen_05_delay_3_reader >= (int)(this->gen_05_delay_3_buffer->getSize()))
        this->gen_05_delay_3_reader = 0;
}

number gen_05_delay_3_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_05_delay_3_buffer->getSize()) + this->gen_05_delay_3_reader - ((size > this->gen_05_delay_3__maxdelay ? this->gen_05_delay_3__maxdelay : (size < (this->gen_05_delay_3_reader != this->gen_05_delay_3_writer) ? this->gen_05_delay_3_reader != this->gen_05_delay_3_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_05_delay_3_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_05_delay_3_wrap))
        ), this->gen_05_delay_3_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_05_delay_3_wrap))
        ));
    }

    number r = (int)(this->gen_05_delay_3_buffer->getSize()) + this->gen_05_delay_3_reader - ((size > this->gen_05_delay_3__maxdelay ? this->gen_05_delay_3__maxdelay : (size < (this->gen_05_delay_3_reader != this->gen_05_delay_3_writer) ? this->gen_05_delay_3_reader != this->gen_05_delay_3_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_05_delay_3_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_05_delay_3_wrap))
    );
}

void gen_05_delay_3_write(number v) {
    this->gen_05_delay_3_writer = this->gen_05_delay_3_reader;
    this->gen_05_delay_3_buffer[(Index)this->gen_05_delay_3_writer] = v;
}

number gen_05_delay_3_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_05_delay_3__maxdelay : size);
    number val = this->gen_05_delay_3_read(effectiveSize, 0);
    this->gen_05_delay_3_write(v);
    this->gen_05_delay_3_step();
    return val;
}

array<Index, 2> gen_05_delay_3_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_05_delay_3_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_05_delay_3_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_05_delay_3_init() {
    auto result = this->gen_05_delay_3_calcSizeInSamples();
    this->gen_05_delay_3__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_05_delay_3_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_05_delay_3_wrap = requestedSizeInSamples - 1;
}

void gen_05_delay_3_clear() {
    this->gen_05_delay_3_buffer->setZero();
}

void gen_05_delay_3_reset() {
    auto result = this->gen_05_delay_3_calcSizeInSamples();
    this->gen_05_delay_3__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_05_delay_3_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_05_delay_3_buffer);
    this->gen_05_delay_3_wrap = this->gen_05_delay_3_buffer->getSize() - 1;
    this->gen_05_delay_3_clear();

    if (this->gen_05_delay_3_reader >= this->gen_05_delay_3__maxdelay || this->gen_05_delay_3_writer >= this->gen_05_delay_3__maxdelay) {
        this->gen_05_delay_3_reader = 0;
        this->gen_05_delay_3_writer = 0;
    }
}

void gen_05_delay_3_dspsetup() {
    this->gen_05_delay_3_reset();
}

number gen_05_delay_3_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_05_delay_3_size() {
    return this->gen_05_delay_3__maxdelay;
}

void gen_05_delay_4_step() {
    this->gen_05_delay_4_reader++;

    if (this->gen_05_delay_4_reader >= (int)(this->gen_05_delay_4_buffer->getSize()))
        this->gen_05_delay_4_reader = 0;
}

number gen_05_delay_4_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_05_delay_4_buffer->getSize()) + this->gen_05_delay_4_reader - ((size > this->gen_05_delay_4__maxdelay ? this->gen_05_delay_4__maxdelay : (size < (this->gen_05_delay_4_reader != this->gen_05_delay_4_writer) ? this->gen_05_delay_4_reader != this->gen_05_delay_4_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_05_delay_4_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_05_delay_4_wrap))
        ), this->gen_05_delay_4_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_05_delay_4_wrap))
        ));
    }

    number r = (int)(this->gen_05_delay_4_buffer->getSize()) + this->gen_05_delay_4_reader - ((size > this->gen_05_delay_4__maxdelay ? this->gen_05_delay_4__maxdelay : (size < (this->gen_05_delay_4_reader != this->gen_05_delay_4_writer) ? this->gen_05_delay_4_reader != this->gen_05_delay_4_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_05_delay_4_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_05_delay_4_wrap))
    );
}

void gen_05_delay_4_write(number v) {
    this->gen_05_delay_4_writer = this->gen_05_delay_4_reader;
    this->gen_05_delay_4_buffer[(Index)this->gen_05_delay_4_writer] = v;
}

number gen_05_delay_4_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_05_delay_4__maxdelay : size);
    number val = this->gen_05_delay_4_read(effectiveSize, 0);
    this->gen_05_delay_4_write(v);
    this->gen_05_delay_4_step();
    return val;
}

array<Index, 2> gen_05_delay_4_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_05_delay_4_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_05_delay_4_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_05_delay_4_init() {
    auto result = this->gen_05_delay_4_calcSizeInSamples();
    this->gen_05_delay_4__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_05_delay_4_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_05_delay_4_wrap = requestedSizeInSamples - 1;
}

void gen_05_delay_4_clear() {
    this->gen_05_delay_4_buffer->setZero();
}

void gen_05_delay_4_reset() {
    auto result = this->gen_05_delay_4_calcSizeInSamples();
    this->gen_05_delay_4__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_05_delay_4_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_05_delay_4_buffer);
    this->gen_05_delay_4_wrap = this->gen_05_delay_4_buffer->getSize() - 1;
    this->gen_05_delay_4_clear();

    if (this->gen_05_delay_4_reader >= this->gen_05_delay_4__maxdelay || this->gen_05_delay_4_writer >= this->gen_05_delay_4__maxdelay) {
        this->gen_05_delay_4_reader = 0;
        this->gen_05_delay_4_writer = 0;
    }
}

void gen_05_delay_4_dspsetup() {
    this->gen_05_delay_4_reset();
}

number gen_05_delay_4_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_05_delay_4_size() {
    return this->gen_05_delay_4__maxdelay;
}

number gen_05_history_5_getvalue() {
    return this->gen_05_history_5_value;
}

void gen_05_history_5_setvalue(number val) {
    this->gen_05_history_5_value = val;
}

void gen_05_history_5_reset() {
    this->gen_05_history_5_value = 0;
}

void gen_05_history_5_init() {
    this->gen_05_history_5_value = 0;
}

number gen_05_history_6_getvalue() {
    return this->gen_05_history_6_value;
}

void gen_05_history_6_setvalue(number val) {
    this->gen_05_history_6_value = val;
}

void gen_05_history_6_reset() {
    this->gen_05_history_6_value = 0;
}

void gen_05_history_6_init() {
    this->gen_05_history_6_value = 0;
}

number gen_05_history_7_getvalue() {
    return this->gen_05_history_7_value;
}

void gen_05_history_7_setvalue(number val) {
    this->gen_05_history_7_value = val;
}

void gen_05_history_7_reset() {
    this->gen_05_history_7_value = 0;
}

void gen_05_history_7_init() {
    this->gen_05_history_7_value = 0;
}

number gen_05_history_8_getvalue() {
    return this->gen_05_history_8_value;
}

void gen_05_history_8_setvalue(number val) {
    this->gen_05_history_8_value = val;
}

void gen_05_history_8_reset() {
    this->gen_05_history_8_value = 0;
}

void gen_05_history_8_init() {
    this->gen_05_history_8_value = 0;
}

number gen_05_history_9_getvalue() {
    return this->gen_05_history_9_value;
}

void gen_05_history_9_setvalue(number val) {
    this->gen_05_history_9_value = val;
}

void gen_05_history_9_reset() {
    this->gen_05_history_9_value = 0;
}

void gen_05_history_9_init() {
    this->gen_05_history_9_value = 0;
}

number gen_05_history_10_getvalue() {
    return this->gen_05_history_10_value;
}

void gen_05_history_10_setvalue(number val) {
    this->gen_05_history_10_value = val;
}

void gen_05_history_10_reset() {
    this->gen_05_history_10_value = 0;
}

void gen_05_history_10_init() {
    this->gen_05_history_10_value = 0;
}

number gen_05_history_11_getvalue() {
    return this->gen_05_history_11_value;
}

void gen_05_history_11_setvalue(number val) {
    this->gen_05_history_11_value = val;
}

void gen_05_history_11_reset() {
    this->gen_05_history_11_value = 0;
}

void gen_05_history_11_init() {
    this->gen_05_history_11_value = 0;
}

number gen_05_history_12_getvalue() {
    return this->gen_05_history_12_value;
}

void gen_05_history_12_setvalue(number val) {
    this->gen_05_history_12_value = val;
}

void gen_05_history_12_reset() {
    this->gen_05_history_12_value = 0;
}

void gen_05_history_12_init() {
    this->gen_05_history_12_value = 0;
}

void gen_05_dspsetup(bool force) {
    if ((bool)(this->gen_05_setupDone) && (bool)(!(bool)(force)))
        return;

    this->gen_05_setupDone = true;
    this->gen_05_delay_3_dspsetup();
    this->gen_05_delay_4_dspsetup();
}

void param_08_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_08_value;
}

void param_08_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_08_value_set(preset["value"]);
}

void numberobj_26_init() {
    this->numberobj_26_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-62"), 1, this->_currentTime);
}

void numberobj_26_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_26_value;
}

void numberobj_26_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_26_value_set(preset["value"]);
}

void numberobj_27_init() {
    this->numberobj_27_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-92"), 1, this->_currentTime);
}

void numberobj_27_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_27_value;
}

void numberobj_27_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_27_value_set(preset["value"]);
}

void numberobj_28_init() {
    this->numberobj_28_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-91"), 1, this->_currentTime);
}

void numberobj_28_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_28_value;
}

void numberobj_28_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_28_value_set(preset["value"]);
}

void numberobj_29_init() {
    this->numberobj_29_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-86"), 1, this->_currentTime);
}

void numberobj_29_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_29_value;
}

void numberobj_29_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_29_value_set(preset["value"]);
}

void numberobj_30_init() {
    this->numberobj_30_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-85"), 1, this->_currentTime);
}

void numberobj_30_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_30_value;
}

void numberobj_30_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_30_value_set(preset["value"]);
}

void numberobj_31_init() {
    this->numberobj_31_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-82"), 1, this->_currentTime);
}

void numberobj_31_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_31_value;
}

void numberobj_31_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_31_value_set(preset["value"]);
}

void numberobj_32_init() {
    this->numberobj_32_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-81"), 1, this->_currentTime);
}

void numberobj_32_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_32_value;
}

void numberobj_32_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_32_value_set(preset["value"]);
}

void numberobj_33_init() {
    this->numberobj_33_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-164"), 1, this->_currentTime);
}

void numberobj_33_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_33_value;
}

void numberobj_33_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_33_value_set(preset["value"]);
}

void param_09_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_09_value;
}

void param_09_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_09_value_set(preset["value"]);
}

void numberobj_34_init() {
    this->numberobj_34_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-99"), 1, this->_currentTime);
}

void numberobj_34_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_34_value;
}

void numberobj_34_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_34_value_set(preset["value"]);
}

void param_10_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_10_value;
}

void param_10_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_10_value_set(preset["value"]);
}

void numberobj_35_init() {
    this->numberobj_35_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-104"), 1, this->_currentTime);
}

void numberobj_35_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_35_value;
}

void numberobj_35_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_35_value_set(preset["value"]);
}

void numberobj_36_init() {
    this->numberobj_36_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-34"), 1, this->_currentTime);
}

void numberobj_36_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_36_value;
}

void numberobj_36_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_36_value_set(preset["value"]);
}

void param_11_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_11_value;
}

void param_11_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_11_value_set(preset["value"]);
}

void numberobj_37_init() {
    this->numberobj_37_currentFormat = 6;
    this->getEngine()->sendNumMessage(TAG("setup"), TAG("number_obj-26"), 1, this->_currentTime);
}

void numberobj_37_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->numberobj_37_value;
}

void numberobj_37_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->numberobj_37_value_set(preset["value"]);
}

void globaltransport_advance() {}

void globaltransport_dspsetup(bool ) {}

bool stackprotect_check() {
    this->stackprotect_count++;

    if (this->stackprotect_count > 128) {
        console->log("STACK OVERFLOW DETECTED - stopped processing branch !");
        return true;
    }

    return false;
}

void updateTime(MillisecondTime time) {
    this->_currentTime = time;
    this->sampleOffsetIntoNextAudioBuffer = (SampleIndex)(rnbo_fround(this->msToSamps(time - this->getEngine()->getCurrentTime(), this->sr)));

    if (this->sampleOffsetIntoNextAudioBuffer >= (SampleIndex)(this->vs))
        this->sampleOffsetIntoNextAudioBuffer = (SampleIndex)(this->vs) - 1;

    if (this->sampleOffsetIntoNextAudioBuffer < 0)
        this->sampleOffsetIntoNextAudioBuffer = 0;
}

void assign_defaults()
{
    dspexpr_01_in1 = 0;
    dspexpr_01_in2 = 1;
    dspexpr_02_in1 = 0;
    dspexpr_03_in1 = 0;
    dspexpr_03_in2 = 0.6;
    gen_01_in1 = 0;
    gen_01_in2 = 0;
    gen_01_in3 = 0;
    gen_01_in4 = 0;
    gen_01_in5 = 0;
    gen_01_in6 = 0;
    gen_01_in7 = 0;
    gen_01_in8 = 0;
    scale_01_inlow = 0;
    scale_01_inhigh = 127;
    scale_01_outlow = 69;
    scale_01_outhigh = 177;
    scale_01_power = 1;
    numberobj_01_value = 0;
    numberobj_01_value_setter(numberobj_01_value);
    numberobj_02_value = 0;
    numberobj_02_value_setter(numberobj_02_value);
    numberobj_03_value = 0;
    numberobj_03_value_setter(numberobj_03_value);
    numberobj_04_value = 0;
    numberobj_04_value_setter(numberobj_04_value);
    numberobj_05_value = 0;
    numberobj_05_value_setter(numberobj_05_value);
    numberobj_06_value = 0;
    numberobj_06_value_setter(numberobj_06_value);
    numberobj_07_value = 0;
    numberobj_07_value_setter(numberobj_07_value);
    param_01_value = 0;
    loadmess_01_message = { 0.4 };
    param_02_value = 0;
    loadmess_02_message = { 1 };
    dspexpr_04_in1 = 0;
    gen_02_in1 = 0;
    gen_02_in2 = 0;
    gen_02_in3 = 0;
    signalgate_01_onoff = 0;
    numberobj_08_value = 0;
    numberobj_08_value_setter(numberobj_08_value);
    signalgate_02_onoff = 0;
    dspexpr_05_in1 = 0;
    dspexpr_05_in2 = 1;
    numberobj_09_value = 0;
    numberobj_09_value_setter(numberobj_09_value);
    gen_03_in1 = 0;
    gen_03_in2 = 0;
    gen_03_in3 = 0;
    gen_03_in4 = 0;
    gen_03_in5 = 0;
    gen_03_in6 = 0;
    loadmess_03_message = { 20 };
    dspexpr_06_in1 = 0;
    dspexpr_07_in1 = 0;
    dspexpr_07_in2 = 0.6;
    gen_04_in1 = 0;
    gen_04_in2 = 0;
    gen_04_in3 = 0;
    gen_04_in4 = 0;
    gen_04_in5 = 0;
    gen_04_in6 = 0;
    gen_04_in7 = 0;
    gen_04_in8 = 0;
    dspexpr_08_in1 = 0;
    dspexpr_08_in2 = 1;
    dspexpr_09_in1 = 0;
    loadmess_04_message = { 110 };
    numberobj_10_value = 0;
    numberobj_10_value_setter(numberobj_10_value);
    numberobj_11_value = 0;
    numberobj_11_value_setter(numberobj_11_value);
    numberobj_12_value = 0;
    numberobj_12_value_setter(numberobj_12_value);
    scale_02_inlow = 0;
    scale_02_inhigh = 127;
    scale_02_outlow = 131;
    scale_02_outhigh = 336;
    scale_02_power = 1;
    numberobj_13_value = 0;
    numberobj_13_value_setter(numberobj_13_value);
    numberobj_14_value = 0;
    numberobj_14_value_setter(numberobj_14_value);
    numberobj_15_value = 0;
    numberobj_15_value_setter(numberobj_15_value);
    numberobj_16_value = 0;
    numberobj_16_value_setter(numberobj_16_value);
    loadmess_05_message = { 1.4 };
    numberobj_17_value = 0;
    numberobj_17_value_setter(numberobj_17_value);
    numberobj_18_value = 0;
    numberobj_18_value_setter(numberobj_18_value);
    numberobj_19_value = 0;
    numberobj_19_value_setter(numberobj_19_value);
    numberobj_20_value = 0;
    numberobj_20_value_setter(numberobj_20_value);
    loadmess_06_message = { 0.7 };
    numberobj_21_value = 0;
    numberobj_21_value_setter(numberobj_21_value);
    loadmess_07_message = { 0.4 };
    numberobj_22_value = 0;
    numberobj_22_value_setter(numberobj_22_value);
    param_03_value = 0;
    param_04_value = 0;
    slider_01_value = 0;
    loadmess_08_message = { 127 };
    numberobj_23_value = 0;
    numberobj_23_value_setter(numberobj_23_value);
    loadmess_09_message = { 15 };
    param_05_value = 0;
    loadmess_10_message = { 0.5 };
    param_06_value = 0;
    loadmess_11_message = { 0.6 };
    numberobj_24_value = 0;
    numberobj_24_value_setter(numberobj_24_value);
    loadmess_12_message = { 1 };
    param_07_value = 0;
    loadmess_13_message = { 43 };
    numberobj_25_value = 0;
    numberobj_25_value_setter(numberobj_25_value);
    dspexpr_10_in1 = 0;
    dspexpr_11_in1 = 0;
    dspexpr_11_in2 = 0.6;
    gen_05_in1 = 0;
    gen_05_in2 = 0;
    gen_05_in3 = 0;
    gen_05_in4 = 0;
    gen_05_in5 = 0;
    gen_05_in6 = 0;
    gen_05_in7 = 0;
    gen_05_in8 = 0;
    param_08_value = 0;
    loadmess_14_message = { 122 };
    numberobj_26_value = 0;
    numberobj_26_value_setter(numberobj_26_value);
    numberobj_27_value = 0;
    numberobj_27_value_setter(numberobj_27_value);
    scale_03_inlow = 0;
    scale_03_inhigh = 127;
    scale_03_outlow = 189;
    scale_03_outhigh = 487;
    scale_03_power = 1;
    numberobj_28_value = 0;
    numberobj_28_value_setter(numberobj_28_value);
    numberobj_29_value = 0;
    numberobj_29_value_setter(numberobj_29_value);
    numberobj_30_value = 0;
    numberobj_30_value_setter(numberobj_30_value);
    numberobj_31_value = 0;
    numberobj_31_value_setter(numberobj_31_value);
    numberobj_32_value = 0;
    numberobj_32_value_setter(numberobj_32_value);
    numberobj_33_value = 0;
    numberobj_33_value_setter(numberobj_33_value);
    loadmess_15_message = { 0.4 };
    param_09_value = 0;
    loadmess_16_message = { 56 };
    numberobj_34_value = 0;
    numberobj_34_value_setter(numberobj_34_value);
    param_10_value = 0;
    loadmess_17_message = { 110 };
    numberobj_35_value = 0;
    numberobj_35_value_setter(numberobj_35_value);
    numberobj_36_value = 0;
    numberobj_36_value_setter(numberobj_36_value);
    param_11_value = 0;
    numberobj_37_value = 0;
    numberobj_37_value_setter(numberobj_37_value);
    _currentTime = 0;
    audioProcessSampleCount = 0;
    sampleOffsetIntoNextAudioBuffer = 0;
    zeroBuffer = nullptr;
    dummyBuffer = nullptr;
    signals[0] = nullptr;
    signals[1] = nullptr;
    signals[2] = nullptr;
    signals[3] = nullptr;
    signals[4] = nullptr;
    signals[5] = nullptr;
    signals[6] = nullptr;
    signals[7] = nullptr;
    signals[8] = nullptr;
    signals[9] = nullptr;
    signals[10] = nullptr;
    signals[11] = nullptr;
    signals[12] = nullptr;
    signals[13] = nullptr;
    signals[14] = nullptr;
    signals[15] = nullptr;
    signals[16] = nullptr;
    signals[17] = nullptr;
    signals[18] = nullptr;
    signals[19] = nullptr;
    signals[20] = nullptr;
    signals[21] = nullptr;
    signals[22] = nullptr;
    didAllocateSignals = 0;
    vs = 0;
    maxvs = 0;
    sr = 44100;
    invsr = 0.00002267573696;
    gen_01_history_1_value = 0;
    gen_01_history_2_value = 0;
    gen_01_delay_3__maxdelay = 0;
    gen_01_delay_3_sizemode = 0;
    gen_01_delay_3_wrap = 0;
    gen_01_delay_3_reader = 0;
    gen_01_delay_3_writer = 0;
    gen_01_delay_4__maxdelay = 0;
    gen_01_delay_4_sizemode = 0;
    gen_01_delay_4_wrap = 0;
    gen_01_delay_4_reader = 0;
    gen_01_delay_4_writer = 0;
    gen_01_history_5_value = 0;
    gen_01_history_6_value = 0;
    gen_01_history_7_value = 0;
    gen_01_history_8_value = 0;
    gen_01_history_9_value = 0;
    gen_01_history_10_value = 0;
    gen_01_history_11_value = 0;
    gen_01_history_12_value = 0;
    gen_01_setupDone = false;
    numberobj_01_currentFormat = 6;
    numberobj_01_lastValue = 0;
    numberobj_02_currentFormat = 6;
    numberobj_02_lastValue = 0;
    numberobj_03_currentFormat = 6;
    numberobj_03_lastValue = 0;
    numberobj_04_currentFormat = 6;
    numberobj_04_lastValue = 0;
    numberobj_05_currentFormat = 6;
    numberobj_05_lastValue = 0;
    numberobj_06_currentFormat = 6;
    numberobj_06_lastValue = 0;
    numberobj_07_currentFormat = 6;
    numberobj_07_lastValue = 0;
    param_01_lastValue = 0;
    param_02_lastValue = 0;
    gen_02_history_1_value = 0;
    gen_02_history_2_value = 0;
    numberobj_08_currentFormat = 6;
    numberobj_08_lastValue = 0;
    numberobj_09_currentFormat = 6;
    numberobj_09_lastValue = 0;
    gen_03_delay_1__maxdelay = 0;
    gen_03_delay_1_sizemode = 0;
    gen_03_delay_1_wrap = 0;
    gen_03_delay_1_reader = 0;
    gen_03_delay_1_writer = 0;
    gen_03_delay_2__maxdelay = 0;
    gen_03_delay_2_sizemode = 0;
    gen_03_delay_2_wrap = 0;
    gen_03_delay_2_reader = 0;
    gen_03_delay_2_writer = 0;
    gen_03_delay_3__maxdelay = 0;
    gen_03_delay_3_sizemode = 0;
    gen_03_delay_3_wrap = 0;
    gen_03_delay_3_reader = 0;
    gen_03_delay_3_writer = 0;
    gen_03_delay_4__maxdelay = 0;
    gen_03_delay_4_sizemode = 0;
    gen_03_delay_4_wrap = 0;
    gen_03_delay_4_reader = 0;
    gen_03_delay_4_writer = 0;
    gen_03_delay_5__maxdelay = 0;
    gen_03_delay_5_sizemode = 0;
    gen_03_delay_5_wrap = 0;
    gen_03_delay_5_reader = 0;
    gen_03_delay_5_writer = 0;
    gen_03_delay_6__maxdelay = 0;
    gen_03_delay_6_sizemode = 0;
    gen_03_delay_6_wrap = 0;
    gen_03_delay_6_reader = 0;
    gen_03_delay_6_writer = 0;
    gen_03_delay_7__maxdelay = 0;
    gen_03_delay_7_sizemode = 0;
    gen_03_delay_7_wrap = 0;
    gen_03_delay_7_reader = 0;
    gen_03_delay_7_writer = 0;
    gen_03_delay_8__maxdelay = 0;
    gen_03_delay_8_sizemode = 0;
    gen_03_delay_8_wrap = 0;
    gen_03_delay_8_reader = 0;
    gen_03_delay_8_writer = 0;
    gen_03_delay_9__maxdelay = 0;
    gen_03_delay_9_sizemode = 0;
    gen_03_delay_9_wrap = 0;
    gen_03_delay_9_reader = 0;
    gen_03_delay_9_writer = 0;
    gen_03_delay_10__maxdelay = 0;
    gen_03_delay_10_sizemode = 0;
    gen_03_delay_10_wrap = 0;
    gen_03_delay_10_reader = 0;
    gen_03_delay_10_writer = 0;
    gen_03_delay_11__maxdelay = 0;
    gen_03_delay_11_sizemode = 0;
    gen_03_delay_11_wrap = 0;
    gen_03_delay_11_reader = 0;
    gen_03_delay_11_writer = 0;
    gen_03_delay_12__maxdelay = 0;
    gen_03_delay_12_sizemode = 0;
    gen_03_delay_12_wrap = 0;
    gen_03_delay_12_reader = 0;
    gen_03_delay_12_writer = 0;
    gen_03_delay_13__maxdelay = 0;
    gen_03_delay_13_sizemode = 0;
    gen_03_delay_13_wrap = 0;
    gen_03_delay_13_reader = 0;
    gen_03_delay_13_writer = 0;
    gen_03_delay_14__maxdelay = 0;
    gen_03_delay_14_sizemode = 0;
    gen_03_delay_14_wrap = 0;
    gen_03_delay_14_reader = 0;
    gen_03_delay_14_writer = 0;
    gen_03_delay_15__maxdelay = 0;
    gen_03_delay_15_sizemode = 0;
    gen_03_delay_15_wrap = 0;
    gen_03_delay_15_reader = 0;
    gen_03_delay_15_writer = 0;
    gen_03_delay_16__maxdelay = 0;
    gen_03_delay_16_sizemode = 0;
    gen_03_delay_16_wrap = 0;
    gen_03_delay_16_reader = 0;
    gen_03_delay_16_writer = 0;
    gen_03_delay_17__maxdelay = 0;
    gen_03_delay_17_sizemode = 0;
    gen_03_delay_17_wrap = 0;
    gen_03_delay_17_reader = 0;
    gen_03_delay_17_writer = 0;
    gen_03_delay_18__maxdelay = 0;
    gen_03_delay_18_sizemode = 0;
    gen_03_delay_18_wrap = 0;
    gen_03_delay_18_reader = 0;
    gen_03_delay_18_writer = 0;
    gen_03_history_19_value = 0;
    gen_03_delay_20__maxdelay = 0;
    gen_03_delay_20_sizemode = 0;
    gen_03_delay_20_wrap = 0;
    gen_03_delay_20_reader = 0;
    gen_03_delay_20_writer = 0;
    gen_03_delay_21__maxdelay = 0;
    gen_03_delay_21_sizemode = 0;
    gen_03_delay_21_wrap = 0;
    gen_03_delay_21_reader = 0;
    gen_03_delay_21_writer = 0;
    gen_03_history_22_value = 0;
    gen_03_delay_23__maxdelay = 0;
    gen_03_delay_23_sizemode = 0;
    gen_03_delay_23_wrap = 0;
    gen_03_delay_23_reader = 0;
    gen_03_delay_23_writer = 0;
    gen_03_delay_24__maxdelay = 0;
    gen_03_delay_24_sizemode = 0;
    gen_03_delay_24_wrap = 0;
    gen_03_delay_24_reader = 0;
    gen_03_delay_24_writer = 0;
    gen_03_history_25_value = 0;
    gen_03_delay_26__maxdelay = 0;
    gen_03_delay_26_sizemode = 0;
    gen_03_delay_26_wrap = 0;
    gen_03_delay_26_reader = 0;
    gen_03_delay_26_writer = 0;
    gen_03_history_27_value = 0;
    gen_03_delay_28__maxdelay = 0;
    gen_03_delay_28_sizemode = 0;
    gen_03_delay_28_wrap = 0;
    gen_03_delay_28_reader = 0;
    gen_03_delay_28_writer = 0;
    gen_03_history_29_value = 0;
    gen_03_delay_30__maxdelay = 0;
    gen_03_delay_30_sizemode = 0;
    gen_03_delay_30_wrap = 0;
    gen_03_delay_30_reader = 0;
    gen_03_delay_30_writer = 0;
    gen_03_history_31_value = 0;
    gen_03_history_32_value = 0;
    gen_03_history_33_value = 0;
    gen_03_cycle_39_ph_currentPhase = 0;
    gen_03_cycle_39_ph_conv = 0;
    gen_03_cycle_39_wrap = 0;
    gen_03_cycle_54_ph_currentPhase = 0;
    gen_03_cycle_54_ph_conv = 0;
    gen_03_cycle_54_wrap = 0;
    gen_03_setupDone = false;
    gen_04_history_1_value = 0;
    gen_04_history_2_value = 0;
    gen_04_delay_3__maxdelay = 0;
    gen_04_delay_3_sizemode = 0;
    gen_04_delay_3_wrap = 0;
    gen_04_delay_3_reader = 0;
    gen_04_delay_3_writer = 0;
    gen_04_delay_4__maxdelay = 0;
    gen_04_delay_4_sizemode = 0;
    gen_04_delay_4_wrap = 0;
    gen_04_delay_4_reader = 0;
    gen_04_delay_4_writer = 0;
    gen_04_history_5_value = 0;
    gen_04_history_6_value = 0;
    gen_04_history_7_value = 0;
    gen_04_history_8_value = 0;
    gen_04_history_9_value = 0;
    gen_04_history_10_value = 0;
    gen_04_history_11_value = 0;
    gen_04_history_12_value = 0;
    gen_04_setupDone = false;
    numberobj_10_currentFormat = 6;
    numberobj_10_lastValue = 0;
    numberobj_11_currentFormat = 6;
    numberobj_11_lastValue = 0;
    numberobj_12_currentFormat = 6;
    numberobj_12_lastValue = 0;
    numberobj_13_currentFormat = 6;
    numberobj_13_lastValue = 0;
    numberobj_14_currentFormat = 6;
    numberobj_14_lastValue = 0;
    numberobj_15_currentFormat = 6;
    numberobj_15_lastValue = 0;
    numberobj_16_currentFormat = 6;
    numberobj_16_lastValue = 0;
    numberobj_17_currentFormat = 6;
    numberobj_17_lastValue = 0;
    metertilde_01_currentPeak = 0;
    metertilde_01_currentInterval = 0;
    metertilde_01_currentIntervalInSamples = 0;
    metertilde_01_setupDone = false;
    numberobj_18_currentFormat = 6;
    numberobj_18_lastValue = 0;
    numberobj_19_currentFormat = 6;
    numberobj_19_lastValue = 0;
    numberobj_20_currentFormat = 6;
    numberobj_20_lastValue = 0;
    numberobj_21_currentFormat = 6;
    numberobj_21_lastValue = 0;
    numberobj_22_currentFormat = 6;
    numberobj_22_lastValue = 0;
    param_03_lastValue = 0;
    param_04_lastValue = 0;
    slider_01_lastValue = 0;
    numberobj_23_currentFormat = 6;
    numberobj_23_lastValue = 0;
    param_05_lastValue = 0;
    param_06_lastValue = 0;
    numberobj_24_currentFormat = 6;
    numberobj_24_lastValue = 0;
    param_07_lastValue = 0;
    numberobj_25_currentFormat = 6;
    numberobj_25_lastValue = 0;
    gen_05_history_1_value = 0;
    gen_05_history_2_value = 0;
    gen_05_delay_3__maxdelay = 0;
    gen_05_delay_3_sizemode = 0;
    gen_05_delay_3_wrap = 0;
    gen_05_delay_3_reader = 0;
    gen_05_delay_3_writer = 0;
    gen_05_delay_4__maxdelay = 0;
    gen_05_delay_4_sizemode = 0;
    gen_05_delay_4_wrap = 0;
    gen_05_delay_4_reader = 0;
    gen_05_delay_4_writer = 0;
    gen_05_history_5_value = 0;
    gen_05_history_6_value = 0;
    gen_05_history_7_value = 0;
    gen_05_history_8_value = 0;
    gen_05_history_9_value = 0;
    gen_05_history_10_value = 0;
    gen_05_history_11_value = 0;
    gen_05_history_12_value = 0;
    gen_05_setupDone = false;
    param_08_lastValue = 0;
    numberobj_26_currentFormat = 6;
    numberobj_26_lastValue = 0;
    numberobj_27_currentFormat = 6;
    numberobj_27_lastValue = 0;
    numberobj_28_currentFormat = 6;
    numberobj_28_lastValue = 0;
    numberobj_29_currentFormat = 6;
    numberobj_29_lastValue = 0;
    numberobj_30_currentFormat = 6;
    numberobj_30_lastValue = 0;
    numberobj_31_currentFormat = 6;
    numberobj_31_lastValue = 0;
    numberobj_32_currentFormat = 6;
    numberobj_32_lastValue = 0;
    numberobj_33_currentFormat = 6;
    numberobj_33_lastValue = 0;
    param_09_lastValue = 0;
    numberobj_34_currentFormat = 6;
    numberobj_34_lastValue = 0;
    param_10_lastValue = 0;
    numberobj_35_currentFormat = 6;
    numberobj_35_lastValue = 0;
    numberobj_36_currentFormat = 6;
    numberobj_36_lastValue = 0;
    param_11_lastValue = 0;
    numberobj_37_currentFormat = 6;
    numberobj_37_lastValue = 0;
    globaltransport_tempo = nullptr;
    globaltransport_state = nullptr;
    stackprotect_count = 0;
    _voiceIndex = 0;
    _noteNumber = 0;
    isMuted = 1;
}

// member variables

    number dspexpr_01_in1;
    number dspexpr_01_in2;
    number dspexpr_02_in1;
    number dspexpr_03_in1;
    number dspexpr_03_in2;
    number gen_01_in1;
    number gen_01_in2;
    number gen_01_in3;
    number gen_01_in4;
    number gen_01_in5;
    number gen_01_in6;
    number gen_01_in7;
    number gen_01_in8;
    list scale_01_input;
    number scale_01_inlow;
    number scale_01_inhigh;
    number scale_01_outlow;
    number scale_01_outhigh;
    number scale_01_power;
    list scale_01_out;
    number numberobj_01_value;
    number numberobj_02_value;
    number numberobj_03_value;
    number numberobj_04_value;
    number numberobj_05_value;
    number numberobj_06_value;
    number numberobj_07_value;
    number param_01_value;
    list loadmess_01_message;
    number param_02_value;
    list loadmess_02_message;
    number dspexpr_04_in1;
    number gen_02_in1;
    number gen_02_in2;
    number gen_02_in3;
    number signalgate_01_onoff;
    number numberobj_08_value;
    number signalgate_02_onoff;
    number dspexpr_05_in1;
    number dspexpr_05_in2;
    number numberobj_09_value;
    number gen_03_in1;
    number gen_03_in2;
    number gen_03_in3;
    number gen_03_in4;
    number gen_03_in5;
    number gen_03_in6;
    list loadmess_03_message;
    number dspexpr_06_in1;
    number dspexpr_07_in1;
    number dspexpr_07_in2;
    number gen_04_in1;
    number gen_04_in2;
    number gen_04_in3;
    number gen_04_in4;
    number gen_04_in5;
    number gen_04_in6;
    number gen_04_in7;
    number gen_04_in8;
    number dspexpr_08_in1;
    number dspexpr_08_in2;
    number dspexpr_09_in1;
    list loadmess_04_message;
    number numberobj_10_value;
    number numberobj_11_value;
    number numberobj_12_value;
    list scale_02_input;
    number scale_02_inlow;
    number scale_02_inhigh;
    number scale_02_outlow;
    number scale_02_outhigh;
    number scale_02_power;
    list scale_02_out;
    number numberobj_13_value;
    number numberobj_14_value;
    number numberobj_15_value;
    number numberobj_16_value;
    list loadmess_05_message;
    number numberobj_17_value;
    number numberobj_18_value;
    number numberobj_19_value;
    number numberobj_20_value;
    list loadmess_06_message;
    number numberobj_21_value;
    list loadmess_07_message;
    number numberobj_22_value;
    number param_03_value;
    number param_04_value;
    number slider_01_value;
    list loadmess_08_message;
    number numberobj_23_value;
    list loadmess_09_message;
    number param_05_value;
    list loadmess_10_message;
    number param_06_value;
    list loadmess_11_message;
    number numberobj_24_value;
    list loadmess_12_message;
    number param_07_value;
    list loadmess_13_message;
    number numberobj_25_value;
    number dspexpr_10_in1;
    number dspexpr_11_in1;
    number dspexpr_11_in2;
    number gen_05_in1;
    number gen_05_in2;
    number gen_05_in3;
    number gen_05_in4;
    number gen_05_in5;
    number gen_05_in6;
    number gen_05_in7;
    number gen_05_in8;
    number param_08_value;
    list loadmess_14_message;
    number numberobj_26_value;
    number numberobj_27_value;
    list scale_03_input;
    number scale_03_inlow;
    number scale_03_inhigh;
    number scale_03_outlow;
    number scale_03_outhigh;
    number scale_03_power;
    list scale_03_out;
    number numberobj_28_value;
    number numberobj_29_value;
    number numberobj_30_value;
    number numberobj_31_value;
    number numberobj_32_value;
    number numberobj_33_value;
    list loadmess_15_message;
    number param_09_value;
    list loadmess_16_message;
    number numberobj_34_value;
    number param_10_value;
    list loadmess_17_message;
    number numberobj_35_value;
    number numberobj_36_value;
    number param_11_value;
    number numberobj_37_value;
    MillisecondTime _currentTime;
    UInt64 audioProcessSampleCount;
    SampleIndex sampleOffsetIntoNextAudioBuffer;
    signal zeroBuffer;
    signal dummyBuffer;
    SampleValue * signals[23];
    bool didAllocateSignals;
    Index vs;
    Index maxvs;
    number sr;
    number invsr;
    number gen_01_history_1_value;
    number gen_01_history_2_value;
    Float64BufferRef gen_01_delay_3_buffer;
    Index gen_01_delay_3__maxdelay;
    Int gen_01_delay_3_sizemode;
    Index gen_01_delay_3_wrap;
    Int gen_01_delay_3_reader;
    Int gen_01_delay_3_writer;
    Float64BufferRef gen_01_delay_4_buffer;
    Index gen_01_delay_4__maxdelay;
    Int gen_01_delay_4_sizemode;
    Index gen_01_delay_4_wrap;
    Int gen_01_delay_4_reader;
    Int gen_01_delay_4_writer;
    number gen_01_history_5_value;
    number gen_01_history_6_value;
    number gen_01_history_7_value;
    number gen_01_history_8_value;
    number gen_01_history_9_value;
    number gen_01_history_10_value;
    number gen_01_history_11_value;
    number gen_01_history_12_value;
    bool gen_01_setupDone;
    Int numberobj_01_currentFormat;
    number numberobj_01_lastValue;
    Int numberobj_02_currentFormat;
    number numberobj_02_lastValue;
    Int numberobj_03_currentFormat;
    number numberobj_03_lastValue;
    Int numberobj_04_currentFormat;
    number numberobj_04_lastValue;
    Int numberobj_05_currentFormat;
    number numberobj_05_lastValue;
    Int numberobj_06_currentFormat;
    number numberobj_06_lastValue;
    Int numberobj_07_currentFormat;
    number numberobj_07_lastValue;
    number param_01_lastValue;
    number param_02_lastValue;
    number gen_02_history_1_value;
    number gen_02_history_2_value;
    Int numberobj_08_currentFormat;
    number numberobj_08_lastValue;
    Int numberobj_09_currentFormat;
    number numberobj_09_lastValue;
    Float64BufferRef gen_03_delay_1_buffer;
    Index gen_03_delay_1__maxdelay;
    Int gen_03_delay_1_sizemode;
    Index gen_03_delay_1_wrap;
    Int gen_03_delay_1_reader;
    Int gen_03_delay_1_writer;
    Float64BufferRef gen_03_delay_2_buffer;
    Index gen_03_delay_2__maxdelay;
    Int gen_03_delay_2_sizemode;
    Index gen_03_delay_2_wrap;
    Int gen_03_delay_2_reader;
    Int gen_03_delay_2_writer;
    Float64BufferRef gen_03_delay_3_buffer;
    Index gen_03_delay_3__maxdelay;
    Int gen_03_delay_3_sizemode;
    Index gen_03_delay_3_wrap;
    Int gen_03_delay_3_reader;
    Int gen_03_delay_3_writer;
    Float64BufferRef gen_03_delay_4_buffer;
    Index gen_03_delay_4__maxdelay;
    Int gen_03_delay_4_sizemode;
    Index gen_03_delay_4_wrap;
    Int gen_03_delay_4_reader;
    Int gen_03_delay_4_writer;
    Float64BufferRef gen_03_delay_5_buffer;
    Index gen_03_delay_5__maxdelay;
    Int gen_03_delay_5_sizemode;
    Index gen_03_delay_5_wrap;
    Int gen_03_delay_5_reader;
    Int gen_03_delay_5_writer;
    Float64BufferRef gen_03_delay_6_buffer;
    Index gen_03_delay_6__maxdelay;
    Int gen_03_delay_6_sizemode;
    Index gen_03_delay_6_wrap;
    Int gen_03_delay_6_reader;
    Int gen_03_delay_6_writer;
    Float64BufferRef gen_03_delay_7_buffer;
    Index gen_03_delay_7__maxdelay;
    Int gen_03_delay_7_sizemode;
    Index gen_03_delay_7_wrap;
    Int gen_03_delay_7_reader;
    Int gen_03_delay_7_writer;
    Float64BufferRef gen_03_delay_8_buffer;
    Index gen_03_delay_8__maxdelay;
    Int gen_03_delay_8_sizemode;
    Index gen_03_delay_8_wrap;
    Int gen_03_delay_8_reader;
    Int gen_03_delay_8_writer;
    Float64BufferRef gen_03_delay_9_buffer;
    Index gen_03_delay_9__maxdelay;
    Int gen_03_delay_9_sizemode;
    Index gen_03_delay_9_wrap;
    Int gen_03_delay_9_reader;
    Int gen_03_delay_9_writer;
    Float64BufferRef gen_03_delay_10_buffer;
    Index gen_03_delay_10__maxdelay;
    Int gen_03_delay_10_sizemode;
    Index gen_03_delay_10_wrap;
    Int gen_03_delay_10_reader;
    Int gen_03_delay_10_writer;
    Float64BufferRef gen_03_delay_11_buffer;
    Index gen_03_delay_11__maxdelay;
    Int gen_03_delay_11_sizemode;
    Index gen_03_delay_11_wrap;
    Int gen_03_delay_11_reader;
    Int gen_03_delay_11_writer;
    Float64BufferRef gen_03_delay_12_buffer;
    Index gen_03_delay_12__maxdelay;
    Int gen_03_delay_12_sizemode;
    Index gen_03_delay_12_wrap;
    Int gen_03_delay_12_reader;
    Int gen_03_delay_12_writer;
    Float64BufferRef gen_03_delay_13_buffer;
    Index gen_03_delay_13__maxdelay;
    Int gen_03_delay_13_sizemode;
    Index gen_03_delay_13_wrap;
    Int gen_03_delay_13_reader;
    Int gen_03_delay_13_writer;
    Float64BufferRef gen_03_delay_14_buffer;
    Index gen_03_delay_14__maxdelay;
    Int gen_03_delay_14_sizemode;
    Index gen_03_delay_14_wrap;
    Int gen_03_delay_14_reader;
    Int gen_03_delay_14_writer;
    Float64BufferRef gen_03_delay_15_buffer;
    Index gen_03_delay_15__maxdelay;
    Int gen_03_delay_15_sizemode;
    Index gen_03_delay_15_wrap;
    Int gen_03_delay_15_reader;
    Int gen_03_delay_15_writer;
    Float64BufferRef gen_03_delay_16_buffer;
    Index gen_03_delay_16__maxdelay;
    Int gen_03_delay_16_sizemode;
    Index gen_03_delay_16_wrap;
    Int gen_03_delay_16_reader;
    Int gen_03_delay_16_writer;
    Float64BufferRef gen_03_delay_17_buffer;
    Index gen_03_delay_17__maxdelay;
    Int gen_03_delay_17_sizemode;
    Index gen_03_delay_17_wrap;
    Int gen_03_delay_17_reader;
    Int gen_03_delay_17_writer;
    Float64BufferRef gen_03_delay_18_buffer;
    Index gen_03_delay_18__maxdelay;
    Int gen_03_delay_18_sizemode;
    Index gen_03_delay_18_wrap;
    Int gen_03_delay_18_reader;
    Int gen_03_delay_18_writer;
    number gen_03_history_19_value;
    Float64BufferRef gen_03_delay_20_buffer;
    Index gen_03_delay_20__maxdelay;
    Int gen_03_delay_20_sizemode;
    Index gen_03_delay_20_wrap;
    Int gen_03_delay_20_reader;
    Int gen_03_delay_20_writer;
    Float64BufferRef gen_03_delay_21_buffer;
    Index gen_03_delay_21__maxdelay;
    Int gen_03_delay_21_sizemode;
    Index gen_03_delay_21_wrap;
    Int gen_03_delay_21_reader;
    Int gen_03_delay_21_writer;
    number gen_03_history_22_value;
    Float64BufferRef gen_03_delay_23_buffer;
    Index gen_03_delay_23__maxdelay;
    Int gen_03_delay_23_sizemode;
    Index gen_03_delay_23_wrap;
    Int gen_03_delay_23_reader;
    Int gen_03_delay_23_writer;
    Float64BufferRef gen_03_delay_24_buffer;
    Index gen_03_delay_24__maxdelay;
    Int gen_03_delay_24_sizemode;
    Index gen_03_delay_24_wrap;
    Int gen_03_delay_24_reader;
    Int gen_03_delay_24_writer;
    number gen_03_history_25_value;
    Float64BufferRef gen_03_delay_26_buffer;
    Index gen_03_delay_26__maxdelay;
    Int gen_03_delay_26_sizemode;
    Index gen_03_delay_26_wrap;
    Int gen_03_delay_26_reader;
    Int gen_03_delay_26_writer;
    number gen_03_history_27_value;
    Float64BufferRef gen_03_delay_28_buffer;
    Index gen_03_delay_28__maxdelay;
    Int gen_03_delay_28_sizemode;
    Index gen_03_delay_28_wrap;
    Int gen_03_delay_28_reader;
    Int gen_03_delay_28_writer;
    number gen_03_history_29_value;
    Float64BufferRef gen_03_delay_30_buffer;
    Index gen_03_delay_30__maxdelay;
    Int gen_03_delay_30_sizemode;
    Index gen_03_delay_30_wrap;
    Int gen_03_delay_30_reader;
    Int gen_03_delay_30_writer;
    number gen_03_history_31_value;
    number gen_03_history_32_value;
    number gen_03_history_33_value;
    number gen_03_cycle_39_ph_currentPhase;
    number gen_03_cycle_39_ph_conv;
    Float64BufferRef gen_03_cycle_39_buffer;
    long gen_03_cycle_39_wrap;
    uint32_t gen_03_cycle_39_phasei;
    SampleValue gen_03_cycle_39_f2i;
    number gen_03_cycle_54_ph_currentPhase;
    number gen_03_cycle_54_ph_conv;
    Float64BufferRef gen_03_cycle_54_buffer;
    long gen_03_cycle_54_wrap;
    uint32_t gen_03_cycle_54_phasei;
    SampleValue gen_03_cycle_54_f2i;
    bool gen_03_setupDone;
    number gen_04_history_1_value;
    number gen_04_history_2_value;
    Float64BufferRef gen_04_delay_3_buffer;
    Index gen_04_delay_3__maxdelay;
    Int gen_04_delay_3_sizemode;
    Index gen_04_delay_3_wrap;
    Int gen_04_delay_3_reader;
    Int gen_04_delay_3_writer;
    Float64BufferRef gen_04_delay_4_buffer;
    Index gen_04_delay_4__maxdelay;
    Int gen_04_delay_4_sizemode;
    Index gen_04_delay_4_wrap;
    Int gen_04_delay_4_reader;
    Int gen_04_delay_4_writer;
    number gen_04_history_5_value;
    number gen_04_history_6_value;
    number gen_04_history_7_value;
    number gen_04_history_8_value;
    number gen_04_history_9_value;
    number gen_04_history_10_value;
    number gen_04_history_11_value;
    number gen_04_history_12_value;
    bool gen_04_setupDone;
    Int numberobj_10_currentFormat;
    number numberobj_10_lastValue;
    Int numberobj_11_currentFormat;
    number numberobj_11_lastValue;
    Int numberobj_12_currentFormat;
    number numberobj_12_lastValue;
    Int numberobj_13_currentFormat;
    number numberobj_13_lastValue;
    Int numberobj_14_currentFormat;
    number numberobj_14_lastValue;
    Int numberobj_15_currentFormat;
    number numberobj_15_lastValue;
    Int numberobj_16_currentFormat;
    number numberobj_16_lastValue;
    Int numberobj_17_currentFormat;
    number numberobj_17_lastValue;
    number metertilde_01_currentPeak;
    SampleIndex metertilde_01_currentInterval;
    SampleIndex metertilde_01_currentIntervalInSamples;
    bool metertilde_01_setupDone;
    Int numberobj_18_currentFormat;
    number numberobj_18_lastValue;
    Int numberobj_19_currentFormat;
    number numberobj_19_lastValue;
    Int numberobj_20_currentFormat;
    number numberobj_20_lastValue;
    Int numberobj_21_currentFormat;
    number numberobj_21_lastValue;
    Int numberobj_22_currentFormat;
    number numberobj_22_lastValue;
    number param_03_lastValue;
    number param_04_lastValue;
    number slider_01_lastValue;
    Int numberobj_23_currentFormat;
    number numberobj_23_lastValue;
    number param_05_lastValue;
    number param_06_lastValue;
    Int numberobj_24_currentFormat;
    number numberobj_24_lastValue;
    number param_07_lastValue;
    Int numberobj_25_currentFormat;
    number numberobj_25_lastValue;
    number gen_05_history_1_value;
    number gen_05_history_2_value;
    Float64BufferRef gen_05_delay_3_buffer;
    Index gen_05_delay_3__maxdelay;
    Int gen_05_delay_3_sizemode;
    Index gen_05_delay_3_wrap;
    Int gen_05_delay_3_reader;
    Int gen_05_delay_3_writer;
    Float64BufferRef gen_05_delay_4_buffer;
    Index gen_05_delay_4__maxdelay;
    Int gen_05_delay_4_sizemode;
    Index gen_05_delay_4_wrap;
    Int gen_05_delay_4_reader;
    Int gen_05_delay_4_writer;
    number gen_05_history_5_value;
    number gen_05_history_6_value;
    number gen_05_history_7_value;
    number gen_05_history_8_value;
    number gen_05_history_9_value;
    number gen_05_history_10_value;
    number gen_05_history_11_value;
    number gen_05_history_12_value;
    bool gen_05_setupDone;
    number param_08_lastValue;
    Int numberobj_26_currentFormat;
    number numberobj_26_lastValue;
    Int numberobj_27_currentFormat;
    number numberobj_27_lastValue;
    Int numberobj_28_currentFormat;
    number numberobj_28_lastValue;
    Int numberobj_29_currentFormat;
    number numberobj_29_lastValue;
    Int numberobj_30_currentFormat;
    number numberobj_30_lastValue;
    Int numberobj_31_currentFormat;
    number numberobj_31_lastValue;
    Int numberobj_32_currentFormat;
    number numberobj_32_lastValue;
    Int numberobj_33_currentFormat;
    number numberobj_33_lastValue;
    number param_09_lastValue;
    Int numberobj_34_currentFormat;
    number numberobj_34_lastValue;
    number param_10_lastValue;
    Int numberobj_35_currentFormat;
    number numberobj_35_lastValue;
    Int numberobj_36_currentFormat;
    number numberobj_36_lastValue;
    number param_11_lastValue;
    Int numberobj_37_currentFormat;
    number numberobj_37_lastValue;
    signal globaltransport_tempo;
    signal globaltransport_state;
    number stackprotect_count;
    DataRef gen_01_delay_3_bufferobj;
    DataRef gen_01_delay_4_bufferobj;
    DataRef gen_03_delay_1_bufferobj;
    DataRef gen_03_delay_2_bufferobj;
    DataRef gen_03_delay_3_bufferobj;
    DataRef gen_03_delay_4_bufferobj;
    DataRef gen_03_delay_5_bufferobj;
    DataRef gen_03_delay_6_bufferobj;
    DataRef gen_03_delay_7_bufferobj;
    DataRef gen_03_delay_8_bufferobj;
    DataRef gen_03_delay_9_bufferobj;
    DataRef gen_03_delay_10_bufferobj;
    DataRef gen_03_delay_11_bufferobj;
    DataRef gen_03_delay_12_bufferobj;
    DataRef gen_03_delay_13_bufferobj;
    DataRef gen_03_delay_14_bufferobj;
    DataRef gen_03_delay_15_bufferobj;
    DataRef gen_03_delay_16_bufferobj;
    DataRef gen_03_delay_17_bufferobj;
    DataRef gen_03_delay_18_bufferobj;
    DataRef gen_03_delay_20_bufferobj;
    DataRef gen_03_delay_21_bufferobj;
    DataRef gen_03_delay_23_bufferobj;
    DataRef gen_03_delay_24_bufferobj;
    DataRef gen_03_delay_26_bufferobj;
    DataRef gen_03_delay_28_bufferobj;
    DataRef gen_03_delay_30_bufferobj;
    DataRef RNBODefaultSinus;
    DataRef gen_04_delay_3_bufferobj;
    DataRef gen_04_delay_4_bufferobj;
    DataRef gen_05_delay_3_bufferobj;
    DataRef gen_05_delay_4_bufferobj;
    Index _voiceIndex;
    Int _noteNumber;
    Index isMuted;
    indexlist paramInitIndices;
    indexlist paramInitOrder;

};

PatcherInterface* creaternbomatic()
{
    return new rnbomatic();
}

#ifndef RNBO_NO_PATCHERFACTORY

extern "C" PatcherFactoryFunctionPtr GetPatcherFactoryFunction(PlatformInterface* platformInterface)
#else

extern "C" PatcherFactoryFunctionPtr rnbomaticFactoryFunction(PlatformInterface* platformInterface)
#endif

{
    Platform::set(platformInterface);
    return creaternbomatic;
}

} // end RNBO namespace

