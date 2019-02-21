/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import * as React from 'react'
import { bindActionCreators, Dispatch } from 'redux'
import { connect } from 'react-redux'

// Components
import Banner from './siteBanner'
import DonationOverlay from 'brave-ui/features/rewards/donationOverlay'

// Utils
import * as rewardsActions from '../actions/donate_actions'

interface Props extends RewardsDonate.ComponentProps {
  publisherKey: string
}

export class App extends React.Component<Props, {}> {

  get actions () {
    return this.props.actions
  }

  onClose = () => {
    this.actions.onCloseDialog()
  }

  generateDonationOverlay = (publisher: RewardsDonate.Publisher) => {
    let domain = ''
    let monthlyDate
    let recurringDonation
    const {
      currentTipAmount,
      currentTipRecurring,
      recurringDonations
    } = this.props.rewardsDonateData

    const publisherKey = publisher && publisher.publisherKey

    if (!publisherKey ||
        (currentTipRecurring && !recurringDonations)) {
      return null
    }

    if (recurringDonations) {
      recurringDonation = recurringDonations.find((donation: RewardsDonate.RecurringDonation) => {
        return donation.publisherKey === publisherKey
      })
    }

    if (recurringDonation && recurringDonation.monthlyDate) {
      monthlyDate = new Date(recurringDonation.monthlyDate * 1000).toLocaleDateString()
    }

    if (publisher.provider && publisher.name) {
      domain = publisher.name
    } else {
      domain = publisherKey
    }

    const verified = publisher.verified
    let logo = publisher.logo

    const internalFavicon = /^https:\/\/[a-z0-9-]+\.invalid(\/)?$/
    if (internalFavicon.test(publisher.logo)) {
      logo = `chrome://favicon/size/160@2x/${publisher.logo}`
    }

    if (!verified) {
      logo = ''
    }

    setTimeout(() => {
      this.onClose()
    }, 3000)

    return (
      <DonationOverlay
        onClose={this.onClose}
        success={true}
        domain={domain}
        amount={currentTipAmount}
        monthlyDate={monthlyDate}
        logo={logo}
      />
    )
  }

  render () {
    const { finished, error, publishers } = this.props.rewardsDonateData

    if (!publishers) {
      return null
    }

    const publisher = publishers[this.props.publisherKey]

    if (!publisher) {
      return null
    }

    return (
      <>
        {
          !finished && !error
          ? <Banner publisher={publisher} />
          : null
        }
        {
          finished
          ? this.generateDonationOverlay(publisher)
          : null
        }
      </>
    )
  }
}

export const mapStateToProps = (state: RewardsDonate.ApplicationState) => ({
  rewardsDonateData: state.rewardsDonateData
})

export const mapDispatchToProps = (dispatch: Dispatch) => ({
  actions: bindActionCreators(rewardsActions, dispatch)
})

export default connect(
  mapStateToProps,
  mapDispatchToProps
)(App)
