def Settings(**kwargs):
  visual_studio = "e:\\programs\\VisualStudio2019"
  windows_sdk = "C:\\Program Files (x86)\\Microsoft SDKs"
  windows_kit = "e:\\Windows Kits"

  includes = [
      '-I%s\\10\\Include\\10.0.19041.0\\ucrt' % windows_kit,
      '-I%s\\Community\\VC\\Tools\\MSVC\\14.29.30133\\include' % visual_studio,
      '-I%s\\MPI\\Include' % windows_sdk,
      '-I%s\\MPI\\Include\\x64' % windows_sdk
      ]
  libs = [
      '-L%s\\Community\\VC\\Tools\\MSVC\\14.29.30133\\lib\\x64' % visual_studio,
      '-L%s\\10\\Lib\\10.0.19041.0\\um\\x64' % windows_kit,
      '-L%s\\10\\Lib\\10.0.19041.0\\ucrt\\x64' % windows_kit,
      '-L%s\\MPI\\Lib\\x64' % windows_sdk
      ]
  flags = ['-x','c++','-Wall','-Wextra','-Werror']
  flags.extend(includes)
  flags.extend(libs)
  flags.append('msmpi.lib')
  return {
      'flags': flags
      }
