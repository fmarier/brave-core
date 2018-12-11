/* This Source Code Form is subject to the terms of the Mozilla Public
 * License. v. 2.0. If a copy of the MPL was not distributed with this file.
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'

// Components
import { AlertBox, Button, Modal, TextAreaClipboard } from 'brave-ui'

// Feature-specific components
import {
  ModalHeader,
  ModalTitle,
  ModalSubTitle,
  ModalContent,
  TwoColumnButtonGrid,
  OneColumnButtonGrid,
  Title,
  SubTitle
} from 'brave-ui/features/sync'

// Utils
import { getLocale } from '../../../../common/locale'
import { getDefaultDeviceName } from '../../helpers'

// Images
import { SyncDefaultIcon } from 'brave-ui/features/sync/images'

interface Props {
  syncData: Sync.State
  actions: any
  onClose: () => void
}
 interface State {
  passphrase: string
  showAlert: boolean
}
 export default class EnterSyncCodeModal extends React.PureComponent<Props, State> {
  constructor (props: Props) {
    super(props)
    this.state = {
      passphrase: '',
      showAlert: false 
    }
  }

  componentWillMount () {
    // sync is initialized in the first screen's render so we can have
    // access to qrcode and sync words. since in this screen user
    // is trying to sync another device, reset the instance created
    // so a new sync chain can be re-created and connect to the remote device
    this.props.actions.onSyncReset()
  }

  onUserNoticedError = () => {
    this.props.actions.resetSyncSetupError()
    this.props.onClose()
  }

  onEnterPassphrase = (event: React.ChangeEvent<HTMLTextAreaElement>) => {
    this.setState({ passphrase: event.target.value })
  }

  onClickConfirmSyncCode = () => {
    const { passphrase } = this.state
    const deviceName = getDefaultDeviceName()
    this.props.actions.onSetupSyncHaveCode(passphrase, deviceName)
  }

  onClickInvalidCode = () => {
    this.setState({ showAlert: false })
  }

  render () {
    const { onClose, syncData } = this.props
    const { showAlert } = this.state

    return (
      <Modal id='enterSyncCodeModal' onClose={onClose} size='small'>
         {
           syncData.error === 'ERR_SYNC_WRONG_WORDS'
           ? <AlertBox okString={getLocale('ok')} onClickOk={this.onUserNoticedError}>
               <Title>{getLocale('errorWrongCodeTitle')}</Title>
               <SubTitle>{getLocale('errorWrongCodeDescription')}</SubTitle>
           </AlertBox>
           : null
         }
         {
           syncData.error === 'ERR_SYNC_MISSING_WORDS'
           ? <AlertBox okString={getLocale('ok')} onClickOk={this.onUserNoticedError}>
               <Title>{getLocale('errorMissingCodeTitle')}</Title>
           </AlertBox>
           : null
         }
         {
           syncData.error === 'ERR_SYNC_NO_INTERNET'
           ? <AlertBox okString={getLocale('ok')} onClickOk={this.onUserNoticedError}>
               <Title>{getLocale('errorNoInternetTitle')}</Title>
               <SubTitle>{getLocale('errorNoInternetDescription')}</SubTitle>
             </AlertBox>
           : null
         }
        {
          syncData.error === 'ERR_SYNC_INIT_FAILED'
          ? <AlertBox okString={getLocale('ok')} onClickOk={this.onUserNoticedError}>
              <Title>{getLocale('errorSyncInitFailedTitle')}</Title>
              <SubTitle>{getLocale('errorSyncInitFailedDescription')}</SubTitle>
            </AlertBox>
          : null
        }
        {
          showAlert
          ? (
              <AlertBox okString={getLocale('ok')} onClickOk={this.onClickInvalidCode}>
                <Title level={1}>{getLocale('invalidCode')}</Title>
                <SubTitle>{getLocale('tryAgain')}</SubTitle>
              </AlertBox>
            )
          : null
        }
        <ModalHeader>
          <SyncDefaultIcon />
          <div>
            <ModalTitle level={1}>{getLocale('enterSyncCode')}</ModalTitle>
            <ModalSubTitle>{getLocale('enterSyncCodeDescription')}</ModalSubTitle>
          </div>
        </ModalHeader>
        <ModalContent>
          <TextAreaClipboard
            copiedString={getLocale('copied')}
            wordCountString={getLocale('wordCount')}
            value={this.state.passphrase}
            onChange={this.onEnterPassphrase}
          />
        </ModalContent>
        <TwoColumnButtonGrid>
          <OneColumnButtonGrid>
            <Button
              level='secondary'
              type='accent'
              size='medium'
              onClick={onClose}
              text={getLocale('cancel')}
            />
          </OneColumnButtonGrid>
          <Button
            level='primary'
            type='accent'
            size='medium'
            onClick={this.onClickConfirmSyncCode}
            text={getLocale('confirmCode')}
          />
        </TwoColumnButtonGrid>
      </Modal>
    )
  }
}
