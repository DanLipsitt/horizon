# Documentation: https://docs.brew.sh/Formula-Cookbook
#                https://rubydoc.brew.sh/Formula
# PLEASE REMOVE ALL GENERATED COMMENTS BEFORE SUBMITTING YOUR PULL REQUEST!
class Horizon < Formula
  desc "Horizon is a free EDA package"
  homepage "https://horizon-eda.org"
  url "https://github.com/horizon-eda/horizon/archive/v1.2.1.tar.gz"
  sha256 "3c0d66afeec55cf7fffbb25a467f7aac1576a5f93fec1f461d7f8a3e20072365"
  head "https://github.com/horizon-eda/horizon.git"

  depends_on "gtkmm3"
  depends_on "adwaita-icon-theme"
  depends_on "cairomm"
  depends_on "librsvg"
  depends_on "yaml-cpp"
  depends_on "sqlite"
  depends_on "boost"
  depends_on "zeromq"
  depends_on "glm"
  depends_on "libepoxy"
  depends_on "libgit2"
  depends_on "curl"
  depends_on "opencascade"
  depends_on "podofo"
  depends_on "libzip"
  depends_on "pkg-config" => :build
  #depends_on "python3" => :build
  #depends_on :x11

  resource "zeromq-cpp" do
    url "https://github.com/zeromq/cppzmq/archive/v4.6.0.tar.gz"
    sha256 "e9203391a0b913576153a2ad22a2dc1479b1ec325beb6c46a3237c669aef5a52"
  end

  def install
    # Tell build where to find Open Cascade
	ENV["CASROOT"] = "#{HOMEBREW_PREFIX}"
    # Download our zeromq-cpp header-only library to make it available for compiling
    resources.each { |r| r.stage(buildpath/"3rd_party"/r.name) }
    (buildpath/"3rd_party").install_symlink "zeromq-cpp/zmq.hpp"

    # build and install
    system "make", "install", "PREFIX=#{prefix}"
  end

  test do
    # `test do` will create, run in and delete a temporary directory.
    #
    # This test will fail and we won't accept that! For Homebrew/homebrew-core
    # this will need to be a test that verifies the functionality of the
    # software. Run the test with `brew test horizon`. Options passed
    # to `brew install` such as `--HEAD` also need to be provided to `brew test`.
    #
    # The installed folder is not in the path, so use the entire path to any
    # executables being tested: `system "#{bin}/program", "do", "something"`.
    system "false"
  end
end
