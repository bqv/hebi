module Plugin where

import Message

import qualified Data.Text as T
import System.Directory
import Data.List
import System.Posix.IO
import System.Posix.Types
import Control.Monad.Reader

import Foreign.C.Types
import Foreign.C.String
import Foreign.Marshal.Alloc

handle :: Message -> ReaderT Fd IO ()
handle (Message _ Invite params) = let
                                    (Short name (Long chans)) = params
                                   in
                                    send ("JOIN "++(T.unpack chans))
handle msg = log_debug "In Haskell" >>
             liftIO (getDirectoryContents "scripts") >>=
             return . filter (isSuffixOf ".hs") >>=
             mapM_ (runScript msg) . map ("./script/"++)

runScript :: Message -> FilePath -> ReaderT Fd IO ()
runScript msg script = log_debug ("Running Script "++script) >>
                       log_debug ("HS-"++(show msg))

foreign import ccall send_hs :: CString -> IO ()

send :: String -> ReaderT Fd IO ()
send s = liftIO (newCString s) >>= \cs ->
            liftIO (send_hs cs) >>
            liftIO (free cs)

log_debug :: String -> ReaderT Fd IO ()
log_debug s = ask >>=
              liftIO . (flip fdWrite $ "+40 "++s) >>
              return ()

log_info :: String -> ReaderT Fd IO ()
log_info s = ask >>=
             liftIO . (flip fdWrite $ "+30 "++s) >>
              return ()

log_warn :: String -> ReaderT Fd IO ()
log_warn s = ask >>=
             liftIO . (flip fdWrite $ "+20 "++s) >>
              return ()

log_error :: String -> ReaderT Fd IO ()
log_error s = ask >>=
              liftIO . (flip fdWrite $ "+10 "++s) >>
              return ()

log_fatal :: String -> ReaderT Fd IO ()
log_fatal s = ask >>=
              liftIO . (flip fdWrite $ "+00 "++s) >>
              return ()

run_hs :: Fd -> Fd -> IO ()
run_hs datafd logfd = runReaderT (log_debug $ "Working with FDs "++(show datafd)++","++(show logfd)) logfd >>
                      forever (fdRead datafd 4096 >>= \(str, _) -> runReaderT (handle $ read str) logfd)
        where
            forever x = x >> forever x

foreign export ccall run_hs :: Fd -> Fd -> IO ()
