/*
 * Copyright 2016, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#define LOG_NDEBUG 0
#define LOG_TAG "BufferImpl"
#include <utils/Log.h>

#include <binder/IMemory.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/ICrypto.h>
#include <utils/NativeHandle.h>

#include "include/SecureBuffer.h"
#include "include/SharedMemoryBuffer.h"

namespace android {

SharedMemoryBuffer::SharedMemoryBuffer(const sp<AMessage> &format, const sp<IMemory> &mem)
    : MediaCodecBuffer(format, new ABuffer(mem->pointer(), mem->size())),
      mMemory(mem) {
}

sp<MediaCodecBuffer> SharedMemoryBuffer::clone(const sp<AMessage> &format) {
    return new SharedMemoryBuffer(format, mMemory);
}

SecureBuffer::SecureBuffer(const sp<AMessage> &format, const void *ptr, size_t size)
    : MediaCodecBuffer(format, new ABuffer(nullptr, size)),
      mPointer(ptr) {
}

SecureBuffer::SecureBuffer(
        const sp<AMessage> &format, const sp<NativeHandle> &handle, size_t size)
    : MediaCodecBuffer(format, new ABuffer(nullptr, size)),
      mPointer(nullptr),
      mHandle(handle) {
}

sp<MediaCodecBuffer> SecureBuffer::clone(const sp<AMessage> &format) {
    return (mHandle == nullptr)
            ? new SecureBuffer(format, mPointer, capacity())
            : new SecureBuffer(format, mHandle, capacity());
}

void *SecureBuffer::getDestinationPointer() {
    return (void *)(mHandle == nullptr ? mPointer : mHandle->handle());
}

ICrypto::DestinationType SecureBuffer::getDestinationType() {
    return mHandle == nullptr ? ICrypto::kDestinationTypeOpaqueHandle
                              : ICrypto::kDestinationTypeNativeHandle;
}

}  // namespace android
